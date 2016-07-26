#include <stdio.h>
#include <string.h>
#include <time.h>

#include "board.h"

/* set the timer starting time */
long u_start_timer() {
	return clock();
}

/* returns the time elapsed (in centiseconds) since the last startTimer call */
int u_stop_timer(long t0) {
	return (clock()-t0)*100/CLOCKS_PER_SEC;
}

char * u_write_move(const move * m, char * s) {
	sprintf(s, "%c%c%c%c",
		(8 - (m->from % 16)) - 1 + 'a', (m->from/16) + '1',
		(8 - (m->to % 16)) - 1 + 'a', (m->to/16) + '1');
	if (m->promotion) {
		switch (m->promotion) {
			case ROOK:
				strcat(s, "r");
				break;
			case KNIGHT:
				strcat(s, "n");
				break;
			case BISHOP:
				strcat(s, "b");
				break;
			case QUEEN:
				strcat(s, "q");
				break;
			default:
				break;
		}
	}
	return s;
}

int u_parse_move(const char * s, move * m) {
	if (strlen(s) >= 4 && strlen(s) <= 5 &&
			s[0] >= 'a' && s[0] <= 'h' && s[1] >= '1' && s[1] <= '8' &&
	        s[2] >= 'a' && s[2] <= 'h' && s[3] >= '1' && s[3] <= '8') {
		m->from = (s[1]-'1')*16+7-s[0]+'a';
		m->to = (s[3]-'1')*16+7-s[2]+'a';
		switch (s[4]) {
			case 'r':
				m->promotion = ROOK;
				break;
			case 'n':
				m->promotion = KNIGHT;
				break;
			case 'b':
				m->promotion = BISHOP;
				break;
			case 'q':
				m->promotion = QUEEN;
				break;
			default:
				m->promotion = NONE;
				break;
		}
		return 1;
	} else {
		m = NULL;
		return 0;
	}
}

void u_bubblesort(move_set * toSort, int r) {
	int i,j,swap=1;
	move m;
	for (i=0; i<r && swap==1; i++) {
		swap = 0;
		for (j=r; j>i; j--) {
			if (toSort->move[j].value > toSort->move[j-1].value) {
				m = toSort->move[j];
				toSort->move[j] = toSort->move[j-1];
				toSort->move[j-1] = m;
				swap = 1;
			}
		}
	}
}

void u_headsort(move_set * toSort, int r, int head) {
	if (head > r) {
		u_bubblesort(toSort, r);
	}
	else {
		move m;
		int i = 0, j = r, pivot = toSort->move[(i+j)/2].value;
		do {
			while (toSort->move[i].value > pivot)
				i++;
			while (toSort->move[j].value < pivot)
				j--;
			if (i<j) {
				m = toSort->move[i];
				toSort->move[i] = toSort->move[j];
				toSort->move[j] = m;
			}
			if (i<=j) {
				i++;
				j--;
			}
		} while (i<=j);
		u_headsort(toSort, j, head);
	}
}
