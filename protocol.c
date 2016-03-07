#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "protocol.h"
#include "search.h"
#include "util.h"

int display_board = 0;
int force_mode = 0;

int p_parse_input(char s[]) {
  char * token = NULL;
  char * string = NULL;
  char delim[] = " ";
  move m;
  string = strtok(s, "\n");
  token = strtok(string, delim);
  if (token == 0)
    return RESULT_OK;
  if (u_parse_move(token, &m)) {
    int i;
    move_set ms;
    b_generate_moves(&ms);
    for (i=0; i<ms.nmoves; i++) {
      if (m.from == ms.move[i].from &&
              m.to == ms.move[i].to &&
              m.promotion == ms.move[i].promotion)
        break;
    }
    if (i == ms.nmoves) {
      return RESULT_BADMOVE;
    }
    b_make_move(&m);
    if (b_under_check(b_get_turn() ^ BLACKORWHITE)) {
      b_unmake_move();
      return RESULT_BADMOVE;
    }
    if (force_mode)
      return RESULT_OK;
    else
      return RESULT_GO;
  }
  if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0) {
    return RESULT_EXIT;
  }
  if (strcmp(token, "force") == 0) {
    force_mode = 1;
    return RESULT_OK;
  }
  if (strcmp(token, "go") == 0) {
    return RESULT_GO;    
  }
  if (strcmp(token, "undo") == 0) {
    b_unmake_move();
    return RESULT_OK;
  }
  if (strcmp(token, "new") == 0) {
    b_reset_board();
    return RESULT_OK;
  }
  if (strcmp(token, "level") == 0) {
    int moves, minutes, seconds, inc;
    token = strtok(NULL, delim);
    sscanf(token, "%d", &moves);
    token = strtok(NULL, delim);
    if (strstr(token, ":")) {
      sscanf(token, "%d:%d", &minutes, &seconds);
      seconds += minutes*60;
    }
    else {
      sscanf(token, "%d", &minutes);
      seconds = minutes*60;
    }
    token = strtok(NULL, delim);
    sscanf(token, "%d", &inc);
    s_set_level(moves, seconds, inc);
    return RESULT_OK;
  }
  if (strcmp(token, "st") == 0) {
    int t;
    token = strtok(NULL, delim);
    sscanf(token, "%d", &t);
    s_set_time(t);
    return RESULT_OK;
  }
  if (strcmp(token, "sd") == 0) {
    int d;
    token = strtok(NULL, delim);
    sscanf(token, "%d", &d);
    s_set_depth(d);
    return RESULT_OK;
  }
  if (strcmp(token, "time") == 0) {
    int t;
    token = strtok(NULL, delim);
    sscanf(token, "%d", &t);
    s_set_clock(t);
    return RESULT_OK;
  }
  if (strcmp(token, "otim") == 0) {
    return RESULT_OK;
  }
  if (strcmp(token, "xboard") == 0) {
    printf("\n");
    return RESULT_OK;
  }
  if (strcmp(token, "perft") == 0) {
    int depth;
    unsigned int counter;
    sscanf(token+6, "%d", &depth);
    counter = s_perft(depth);
    printf("perft(%d) = %u\n", depth, counter);
    return RESULT_OK;
  }
  if (strcmp(token, "d") == 0) {
    b_print_board();
    return RESULT_OK;
  }
  return RESULT_BADCOMMAND;
}

