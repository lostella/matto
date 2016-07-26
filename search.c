#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "board.h"
#include "input.h"
#include "search.h"
#include "util.h"

int exact_time = 1500;
int exact_depth = 0;

int time_left = 0;
int level_moves = 0;
int level_time = 0;
int level_inc = 0;

int ab_nodes;
int q_nodes;
int ab_calls;

move killer_heuristic[MAX_PLIES];

int nodes_per_second = 50000;
int next_time_check = DEFAULT_NPS/TIME_CHECKS_PER_SECOND;
int abort_search = 0;

long start;

int quiescent_search(int alpha, int beta) {
	int i, val;
	move_set moves;

	val = b_evaluate();

	if (val >= beta)
		return beta;
	if (val > alpha)
		alpha = val;

	b_generate_captures(&moves);

	if (!moves.nmoves)
		return val;

	b_sort_moves(&moves, FLAG_EVAL);

	for (i=0; i<moves.nmoves; i++) {
		b_make_move(&moves.move[i]);

		if (b_under_check(b_get_turn()^BLACKORWHITE)) {
			b_unmake_move();
			continue;
		}

		q_nodes++;

		val = -quiescent_search(-beta, -alpha);

		b_unmake_move();

		if (val >= beta) {
			return beta;
		}
		if (val > alpha) {
			alpha = val;
		}
	}

	return alpha;
}

int alphabeta_search(int alpha, int beta, int depth, move_set * moves, move_set * variation) {
	move_set np_moves;
	move_set np_variation;
	int i, j, val, legal_moves = 0;

	ab_calls++;

	/* substitute this with a timer? */
	if (next_time_check-- <= 0) {
		if (exact_time && u_stop_timer(start) >= exact_time)
			abort_search = 1;
		next_time_check = (int)(nodes_per_second/TIME_CHECKS_PER_SECOND);
	}
	else if (i_tryp_full() != EBUSY) {
		if (strcmp(i_get_buffer(), "?\n") == 0)
			abort_search = 1;
		i_v_empty();
	}

	if (abort_search) {
		variation->nmoves = 0;
		return 0;
	}

	for (i=0; i<moves->nmoves; i++) {
		b_make_move(&moves->move[i]);

		/* go to next move if this one is illegal */
		if (b_under_check(b_get_turn()^BLACKORWHITE)) {
			b_unmake_move();
			continue;
		}

		legal_moves++;
		ab_nodes++;

		/* check for a draw game */
		if (b_count_repetitions() >= 3 || b_get_fifty_counter() == 50) {
			val = DRAW;
			np_variation.nmoves = 0;
		}
		/* we can go further down in the tree */
		if (depth > 1) {
			b_generate_moves(&np_moves); // generate next level moves
			b_sort_moves(&np_moves, FLAG_EVAL);
			val = -alphabeta_search(-beta, -alpha, depth-1, &np_moves, &np_variation);
		}
		/* evaluate current position */
		else {
			val = -quiescent_search(-beta, -alpha);
			np_variation.nmoves = 0;
		}

		b_unmake_move();

		if (abort_search) {
			if (!i)	// if only one move was tried in this position
				return INVALID;	// then nothing can be said
			break; // otherwise return the value found
		}

		moves->move[i].value = val;

		/* too strong a move, branch */
		if (val >= beta) {
			killer_heuristic[b_get_ply_counter()] = moves->move[i];	/* annota la mossa che ha generato il taglio */
			return beta;
		}
		/* we found a better move */
		if (val > alpha) {
			alpha = val;
			moves->move[i].value++;							/* in modo da riconoscere a livello superiore la mossa tra quelle di pari valore */
			variation->move[0] = moves->move[i];			/* inserisce la mossa in testa alla linea di gioco di questo ramo */
			for (j=0; j<np_variation.nmoves; j++)			/* e ci accoda la linea di gioco successiva alla mossa in testa */
				variation->move[j+1] = np_variation.move[j];
			variation->nmoves = np_variation.nmoves+1;		/* imposta la lunghezza della variazione */
		}
	}

	/* if there is no legal moves here */
	if (!legal_moves) {
		/* checkmate here */
		if (b_under_check(b_get_turn()))
			return -INFINITY;
		/* draw here */
		return DRAW;
	}

	return alpha;
}

void s_best_move(move * m, int output) {
	int i=1, j;
	int val, lower_bound = -INFINITY, upper_bound = +INFINITY;
	int t;
	char s[5];
	move_set moves;
	move_set variation;

	if (time_left) {
		if (level_moves)
			exact_time = time_left/(level_moves - (b_get_ply_counter()/2)%level_moves) + level_inc/2 - 20;
		else
			exact_time = time_left/20 + level_inc/2;
	}

	ab_nodes = q_nodes = ab_calls = abort_search = 0;
	next_time_check = (int)(nodes_per_second/TIME_CHECKS_PER_SECOND);
	start = u_start_timer();
	b_generate_moves(&moves);

	while (!abort_search) {
		val = alphabeta_search(lower_bound, upper_bound, i, &moves, &variation);
		b_sort_moves(&moves, FLAG_ALL);

		if (val == INVALID)
			continue;

		if (val <= lower_bound) {
			lower_bound -= WINDOW;
			continue;
		}
		if (val >= upper_bound) {
			upper_bound += WINDOW;
			continue;
		}

		lower_bound = val - WINDOW;
		upper_bound = val + WINDOW;

		nodes_per_second = (int)((float)(100*(ab_nodes+q_nodes))/u_stop_timer(start));

		if (output) {
			printf("%d %d %d %d ", i, val, u_stop_timer(start), ab_nodes+q_nodes);
			for (j=0; j<variation.nmoves; j++)
				printf("%s ", u_write_move(&variation.move[j], s));
			printf("\n");
			fflush(stdout);
		}

		i++;

		if (val >= INFINITY-i || val <= -INFINITY+i)
			abort_search = 1;
		if (exact_depth && i > exact_depth)
			abort_search = 1;
	}

	t = u_stop_timer(start);

	*m = variation.move[0];
}

void s_set_time(int t) {
	exact_time = t*100;
	time_left = 0;
	exact_depth = 0;
}

void s_set_depth(int d) {
	exact_depth = d;
	exact_time = 0;
	time_left = 0;
}

void s_set_clock(int t) {
	time_left = t;
	exact_time = 0;
	exact_depth = 0;
}

void s_set_level(int moves, int time, int inc) {
	level_moves = moves;
	level_time = time_left = time*100;
	level_inc = inc*100;
}

move * s_get_killer(int ply) {
	return &killer_heuristic[ply];
}

u64 s_perft(int depth) {
	int i;
	u64 count = 0;
	move_set ms;

	if (!depth)
		return 1;
	b_generate_moves(&ms);
	for (i=0; i<ms.nmoves; i++) {
		b_make_move(&ms.move[i]);
		if (b_under_check(b_get_turn()^BLACKORWHITE)) {
			b_unmake_move();
			continue;
		}
		count += s_perft(depth-1);
		b_unmake_move();
	}
	return count;
}
