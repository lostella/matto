#ifndef UTIL_H_
#define UTIL_H_

#include "board.h"

long u_start_timer();
int u_stop_timer(long);
char *u_write_move(const move *, char *);
int u_parse_move(const char *, move *);
void u_bubblesort(move_set *, int);
void u_headsort(move_set *, int, int);

#endif /*UTIL_H_*/
