#ifndef _BOARD_H_
#define _BOARD_H_

#define MOV_PAWN 0x01   /* 0 0 0 0 0 0 0 1 */
#define MOV_0_90 0x02   /* 0 0 0 0 0 0 1 0 */
#define MOV_45_135 0x04 /* 0 0 0 0 0 1 0 0 */
#define MOV_L 0x08      /* 0 0 0 0 1 0 0 0 */
#define MOV_SLIDES 0x10 /* 0 0 0 1 0 0 0 0 */

#define WHITE 0x20        /* 0 0 1 0 0 0 0 0 */
#define BLACK 0x40        /* 0 1 0 0 0 0 0 0 */
#define BLACKORWHITE 0x60 /* 0 1 1 0 0 0 0 0 */

#define PAWN 0x01   /* 0 0 0 0 0 0 0 1 */
#define ROOK 0x12   /* 0 0 0 1 0 0 1 0 */
#define KNIGHT 0x08 /* 0 0 0 0 1 0 0 0 */
#define BISHOP 0x14 /* 0 0 0 1 0 1 0 0 */
#define QUEEN 0x16  /* 0 0 0 1 0 1 1 0 */
#define KING 0x06   /* 0 0 0 0 0 1 1 0 */
#define PIECE 0x1F  /* 0 0 0 1 1 1 1 1 */

#define NONE -1

#define CASTLE_W_OO 0x01
#define CASTLE_W_OOO 0x02
#define CASTLE_W 0x03
#define CASTLE_B_OO 0x04
#define CASTLE_B_OOO 0x08
#define CASTLE_B 0x0C

#define ENPASSANT 0x80

#define MAX_PLIES 256
#define SET_SIZE 256

#define FLAG_NONE 0x00
#define FLAG_EVAL 0x01
#define FLAG_ALL 0x02

typedef struct piece
{
  int type;
  int location;
} piece;

typedef struct move
{
  int from;
  int to;
  int promotion;
  int value;
} move;

typedef struct move_set
{
  move move[SET_SIZE];
  int nmoves;
} move_set;

int b_count_repetitions();
int b_evaluate();
void b_generate_captures(move_set *);
void b_generate_moves(move_set *);
int b_get_fifty_counter();
int b_get_ply_counter();
int b_get_turn();
void b_make_move(const move *);
void b_print_board();
void b_reset_board();
void b_sort_moves(move_set *, int);
int b_under_check(int);
void b_unmake_move();

#endif /*_BOARD_H_*/
