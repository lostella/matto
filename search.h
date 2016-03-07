#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "board.h"

#define TIME_CHECKS_PER_SECOND	30
#define DEFAULT_NPS				50000
#define WINDOW					100
#define DRAW					0
#define INFINITY				9999
#define INVALID					-INFINITY-1

typedef unsigned long long u64;

void		s_best_move			(move *, int);
move	*	s_get_killer		(int);
u64			s_perft				(int);
void		s_set_time			(int);
void		s_set_depth			(int);
void		s_set_clock			(int);
void		s_set_level			(int, int, int);

#endif /*SEARCH_H_*/
