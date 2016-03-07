#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "protocol.h"
#include "input.h"
#include "util.h"
#include "search.h"

int main(int argc, char* argv[]) {
  char c[6];
  int result, game_over = 0;
  move m;
  
  /* avvia il thread che acquisisce l'input*/
  i_start_input_thread();
  
  /* inizializza la scacchiera */
  b_reset_board();

  do {
    /* svuota il buffer di output */
    fflush(stdout);

    /* attende input pieno */
    i_p_full();
    
    /* esamina l'input acquisito */
    result = p_parse_input(i_get_buffer());
    
    /* controlla l'esito dell'ultimo input */
    switch (result) {
      // comando regolare
      case RESULT_OK:
        i_v_empty();
        break;
      // comando di uscita
      case RESULT_EXIT:
        game_over = 1;
        i_stop_input_thread();
        break;
      // tocca al computer giocare
      case RESULT_GO:
        i_v_empty();
        s_best_move(&m, 1);
        b_make_move(&m);
        printf("move %s\n", u_write_move(&m, c));
        break;
      // comando errato
      case RESULT_BADCOMMAND:
        printf("error: %s\n", i_get_buffer());
        i_v_empty();
        break;
      // mossa illegale
      case RESULT_BADMOVE:
        printf("illegal move: %s\n", i_get_buffer());
        i_v_empty();
        break;
    }
    
    /* comunica buffer vuoto */
    
  } while (!game_over);

  return EXIT_SUCCESS;
}
