#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "input.h"
#include "protocol.h"
#include "search.h"
#include "util.h"

int main(int argc, char *argv[]) {
  char c[6];
  int result, game_over = 0;
  move m;

  // start input thread
  i_start_input_thread();

  // initialize board
  b_reset_board();

  do {
    // empty input buffer
    fflush(stdout);

    // wait for full buffer
    i_p_full();

    // parse the input string
    result = p_parse_input(i_get_buffer());

    switch (result) {
    // wrong command
    case RESULT_BADCOMMAND:
      printf("error: %s\n", i_get_buffer());
      break;
    // illegal move
    case RESULT_BADMOVE:
      printf("illegal move: %s\n", i_get_buffer());
      break;
    }

    i_v_empty();

    switch (result) {
    // exit command
    case RESULT_EXIT:
      game_over = 1;
      i_stop_input_thread();
      break;
    // computer should move
    case RESULT_GO:
      s_best_move(&m, 1);
      b_make_move(&m);
      printf("move %s\n", u_write_move(&m, c));
      break;
    }

  } while (!game_over);

  return EXIT_SUCCESS;
}
