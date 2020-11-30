#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "search.h"
#include "util.h"

#define HEAD 10

int turn;
int ply_counter;

int castle[MAX_PLIES];
int en_passant[MAX_PLIES];
int count_50[MAX_PLIES];
piece *trash[MAX_PLIES];
move history[MAX_PLIES];

piece wpieces[16]; /* white pieces */
piece bpieces[16]; /* black pieces */
piece *board[128]; /* the chessboard */

/*
    invalid positions                 valid positions

    127 126 125 124 123 122 121 120 | 119 118 117 116 115 114 113 112
    111 110 109 108 107 106 105 104 | 103 102 101 100  99  98  97  96
     95  94  93  92  91  90  89  88 |  87  86  85  84  83  82  81  80
     79  78  77  76  75  74  73  72 |  71  70  69  68  67  66  65  64
     63  62  61  60  59  58  57  56 |  55  54  53  52  51  50  49  48
     47  46  45  44  43  42  41  40 |  39  38  37  36  35  34  33  32
     31  30  29  28  27  26  25  24 |  23  22  21  20  19  18  17  16
     15  14  13  12  11  10   9   8 |   7   6   5   4   3   2   1   0
*/

const int castle_mask[128] = {
    14, 15, 15, 12, 15, 15, 15, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 11, 15,
    15, 3,  15, 15, 15, 7,  15, 15, 15, 15, 15, 15, 15, 15};

int b_attack(int color, int location) {
  if (color == WHITE && location >= 32) {
    if (!((location - 15) & 0x88) && board[location - 15] &&
        board[location - 15]->type == (PAWN | WHITE))
      return 1;
    if (!((location - 17) & 0x88) && board[location - 17] &&
        board[location - 17]->type == (PAWN | WHITE))
      return 1;
  }
  if (color == BLACK && location <= 87) {
    if (!((location + 15) & 0x88) && board[location + 15] &&
        board[location + 15]->type == (PAWN | BLACK))
      return 1;
    if (!((location + 17) & 0x88) && board[location + 17] &&
        board[location + 17]->type == (PAWN | BLACK))
      return 1;
  }
  if (!((location + 33) & 0x88) && board[location + 33] &&
      board[location + 33]->type == (KNIGHT | color))
    return 1;
  if (!((location + 31) & 0x88) && board[location + 31] &&
      board[location + 31]->type == (KNIGHT | color))
    return 1;
  if (!((location + 14) & 0x88) && board[location + 14] &&
      board[location + 14]->type == (KNIGHT | color))
    return 1;
  if (!((location + 18) & 0x88) && board[location + 18] &&
      board[location + 18]->type == (KNIGHT | color))
    return 1;
  if (!((location - 33) & 0x88) && board[location - 33] &&
      board[location - 33]->type == (KNIGHT | color))
    return 1;
  if (!((location - 31) & 0x88) && board[location - 31] &&
      board[location - 31]->type == (KNIGHT | color))
    return 1;
  if (!((location - 14) & 0x88) && board[location - 14] &&
      board[location - 14]->type == (KNIGHT | color))
    return 1;
  if (!((location - 18) & 0x88) && board[location - 18] &&
      board[location - 18]->type == (KNIGHT | color))
    return 1;
  int d = location;
  do {
    d++;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_0_90) {
        if (!(board[d]->type & MOV_SLIDES) && d - location > 1)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d--;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_0_90) {
        if (!(board[d]->type & MOV_SLIDES) && location - d > 1)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d += 16;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_0_90) {
        if (!(board[d]->type & MOV_SLIDES) && d - location > 16)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d -= 16;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_0_90) {
        if (!(board[d]->type & MOV_SLIDES) && location - d > 16)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d += 15;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_45_135) {
        if (!(board[d]->type & MOV_SLIDES) && d - location > 15)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d -= 15;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_45_135) {
        if (!(board[d]->type & MOV_SLIDES) && location - d > 15)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d += 17;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_45_135) {
        if (!(board[d]->type & MOV_SLIDES) && d - location > 17)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  d = location;
  do {
    d -= 17;
    if (d & 0x88)
      break;
    if (board[d] == 0) {
      continue;
    }
    if (board[d]->type & color) {
      if (board[d]->type & MOV_45_135) {
        if (!(board[d]->type & MOV_SLIDES) && location - d > 17)
          break;
        else
          return 1;
      } else
        break;
    } else
      break;
  } while (1);
  return 0;
}

void b_insert_move(int from, int to, int promotion, move_set *tofill) {
  move *move_ptr = &(tofill->move[tofill->nmoves]);
  if (promotion && (to > 111 || to < 8)) {
    move_ptr[0].from = from;
    move_ptr[0].to = to;
    move_ptr[0].promotion = ROOK;
    move_ptr[1].from = from;
    move_ptr[1].to = to;
    move_ptr[1].promotion = KNIGHT;
    move_ptr[2].from = from;
    move_ptr[2].to = to;
    move_ptr[2].promotion = BISHOP;
    move_ptr[3].from = from;
    move_ptr[3].to = to;
    move_ptr[3].promotion = QUEEN;
    tofill->nmoves += 4;
  } else {
    move_ptr->from = from;
    move_ptr->to = to;
    move_ptr->promotion = NONE;
    tofill->nmoves++;
  }
}

void b_generate_moves(move_set *mset) {
  piece *pset;
  int i, l;

  mset->nmoves = 0;

  if (turn & WHITE)
    pset = wpieces;
  else
    pset = bpieces;

  for (i = 0; i < 16; i++) {
    l = pset[i].location;
    if (l == NONE)
      continue;
    if (pset[i].type & MOV_PAWN) {
      /* pawn: white = {16, 32, 15, 17}, black = {-16, -32, -15, -17} */
      if (turn & WHITE) {
        if (!((l + 16) & 0x88) && board[l + 16] == 0) {
          b_insert_move(l, l + 16, 1, mset);
          if (l < 24 && board[l + 32] == 0)
            b_insert_move(l, l + 32, 0, mset);
        }
        if (!((l + 15) & 0x88) &&
            ((board[l + 15] && board[l + 15]->type & BLACK) ||
             l + 15 == en_passant[ply_counter]))
          b_insert_move(l, l + 15, 1, mset);
        if (!((l + 17) & 0x88) &&
            ((board[l + 17] && board[l + 17]->type & BLACK) ||
             l + 17 == en_passant[ply_counter]))
          b_insert_move(l, l + 17, 1, mset);
      } else {
        if (!((l - 16) & 0x88) && board[l - 16] == 0) {
          b_insert_move(l, l - 16, 1, mset);
          if (l > 95 && board[l - 32] == 0)
            b_insert_move(l, l - 32, 0, mset);
        }
        if (!((l - 15) & 0x88) &&
            ((board[l - 15] && board[l - 15]->type & WHITE) ||
             l - 15 == en_passant[ply_counter]))
          b_insert_move(l, l - 15, 1, mset);
        if (!((l - 17) & 0x88) &&
            ((board[l - 17] && board[l - 17]->type & WHITE) ||
             l - 17 == en_passant[ply_counter]))
          b_insert_move(l, l - 17, 1, mset);
      }
      continue;
    }
    if (pset[i].type & MOV_L) {
      /* knight: {33, 31, 14, 18, -18, -14, -31, -33} */
      if (!((l + 33) & 0x88) &&
          (board[l + 33] == 0 || !(board[l + 33]->type & turn)))
        b_insert_move(l, l + 33, 0, mset);
      if (!((l + 31) & 0x88) &&
          (board[l + 31] == 0 || !(board[l + 31]->type & turn)))
        b_insert_move(l, l + 31, 0, mset);
      if (!((l + 14) & 0x88) &&
          (board[l + 14] == 0 || !(board[l + 14]->type & turn)))
        b_insert_move(l, l + 14, 0, mset);
      if (!((l + 18) & 0x88) &&
          (board[l + 18] == 0 || !(board[l + 18]->type & turn)))
        b_insert_move(l, l + 18, 0, mset);
      if (!((l - 33) & 0x88) &&
          (board[l - 33] == 0 || !(board[l - 33]->type & turn)))
        b_insert_move(l, l - 33, 0, mset);
      if (!((l - 31) & 0x88) &&
          (board[l - 31] == 0 || !(board[l - 31]->type & turn)))
        b_insert_move(l, l - 31, 0, mset);
      if (!((l - 14) & 0x88) &&
          (board[l - 14] == 0 || !(board[l - 14]->type & turn)))
        b_insert_move(l, l - 14, 0, mset);
      if (!((l - 18) & 0x88) &&
          (board[l - 18] == 0 || !(board[l - 18]->type & turn)))
        b_insert_move(l, l - 18, 0, mset);
      continue;
    }
    if (pset[i].type & MOV_0_90) {
      /* 0_90: {1, -1, 16, -16} */
      int d = l;
      do {
        d++;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d--;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d += 16;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 16;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
    }
    if (pset[i].type & MOV_45_135) {
      /* 45_135: {17, 15, -17, -15} */
      int d = l;
      do {
        d += 17;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d += 15;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 17;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 15;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          b_insert_move(l, d, 0, mset);
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
    }
  }
  if (turn == WHITE) {
    if ((castle[ply_counter] & CASTLE_W_OO) && !board[1] && !board[2] &&
        !b_attack(BLACK, 3) && !b_attack(BLACK, 2) && !b_attack(BLACK, 1))
      b_insert_move(3, 1, 0, mset);

    if ((castle[ply_counter] & CASTLE_W_OOO) && !board[4] && !board[5] &&
        !board[6] && !b_attack(BLACK, 3) && !b_attack(BLACK, 4) &&
        !b_attack(BLACK, 5))
      b_insert_move(3, 5, 0, mset);
  } else {
    if ((castle[ply_counter] & CASTLE_B_OO) && !board[114] && !board[113] &&
        !b_attack(WHITE, 115) && !b_attack(WHITE, 114) && !b_attack(WHITE, 113))
      b_insert_move(115, 113, 0, mset);

    if ((castle[ply_counter] & CASTLE_B_OOO) && !board[116] && !board[117] &&
        !board[118] && !b_attack(WHITE, 115) && !b_attack(WHITE, 116) &&
        !b_attack(WHITE, 117))
      b_insert_move(115, 117, 0, mset);
  }
}

void b_generate_captures(move_set *mset) {
  piece *pset;
  int i, l;

  mset->nmoves = 0;

  if (turn & WHITE)
    pset = wpieces;
  else
    pset = bpieces;

  for (i = 0; i < 16; i++) {
    l = pset[i].location;
    if (l == NONE)
      continue;
    if (pset[i].type & MOV_PAWN) {
      /* pawn: white = {16, 32, 15, 17}, black = {-16, -32, -15, -17} */
      if (turn & WHITE) {
        if (!((l + 15) & 0x88) &&
            ((board[l + 15] && board[l + 15]->type & BLACK) ||
             l + 15 == en_passant[ply_counter]))
          b_insert_move(l, l + 15, 1, mset);
        if (!((l + 17) & 0x88) &&
            ((board[l + 17] && board[l + 17]->type & BLACK) ||
             l + 17 == en_passant[ply_counter]))
          b_insert_move(l, l + 17, 1, mset);
      } else {
        if (!((l - 15) & 0x88) &&
            ((board[l - 15] && board[l - 15]->type & WHITE) ||
             l - 15 == en_passant[ply_counter]))
          b_insert_move(l, l - 15, 1, mset);
        if (!((l - 17) & 0x88) &&
            ((board[l - 17] && board[l - 17]->type & WHITE) ||
             l - 17 == en_passant[ply_counter]))
          b_insert_move(l, l - 17, 1, mset);
      }
      continue;
    }
    if (pset[i].type & MOV_L) {
      /* knight: {33, 31, 14, 18, -18, -14, -31, -33} */
      if (!((l + 33) & 0x88) && board[l + 33] &&
          (board[l + 33]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l + 33, 0, mset);
      if (!((l + 31) & 0x88) && board[l + 31] &&
          (board[l + 31]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l + 31, 0, mset);
      if (!((l + 14) & 0x88) && board[l + 14] &&
          (board[l + 14]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l + 14, 0, mset);
      if (!((l + 18) & 0x88) && board[l + 18] &&
          (board[l + 18]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l + 18, 0, mset);
      if (!((l - 33) & 0x88) && board[l - 33] &&
          (board[l - 33]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l - 33, 0, mset);
      if (!((l - 31) & 0x88) && board[l - 31] &&
          (board[l - 31]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l - 31, 0, mset);
      if (!((l - 14) & 0x88) && board[l - 14] &&
          (board[l - 14]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l - 14, 0, mset);
      if (!((l - 18) & 0x88) && board[l - 18] &&
          (board[l - 18]->type & (turn ^ BLACKORWHITE)))
        b_insert_move(l, l - 18, 0, mset);
      continue;
    }
    if (pset[i].type & MOV_0_90) {
      /* 0_90: {1, -1, 16, -16} */
      int d = l;
      do {
        d++;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d--;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d += 16;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 16;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
    }
    if (pset[i].type & MOV_45_135) {
      /* 45_135: {17, 15, -17, -15} */
      int d = l;
      do {
        d += 17;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d += 15;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 17;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
      d = l;
      do {
        d -= 15;
        if (d & 0x88)
          break;
        if (board[d] == 0) {
          if (pset[i].type & MOV_SLIDES)
            continue;
          break;
        }
        if ((board[d]->type & BLACKORWHITE) == turn)
          break;
        else {
          b_insert_move(l, d, 0, mset);
          break;
        }
      } while (1);
    }
  }
}

int b_count_repetitions() {
  if (count_50[ply_counter] <= 3) {
    return 0;
  } else {
    int i, c = 0, r = 0;
    int b[128];
    for (i = 0; i < 128; i++)
      b[i] = 0;
    for (i = ply_counter; i > ply_counter - count_50[ply_counter]; i--) {
      if (++b[history[i].from] == 0)
        --c;
      else
        ++c;
      if (--b[history[i].to] == 0)
        --c;
      else
        ++c;
      if (c == 0)
        ++r;
    }
    return r;
  }
}

int b_under_check(int color) {
  piece *ps;
  int i;
  if (color == WHITE)
    ps = wpieces;
  else
    ps = bpieces;
  for (i = 0; i < 16; i++)
    if (ps[i].type == (KING | color)) {
      if (b_attack(color ^ BLACKORWHITE, ps[i].location))
        return 1;
      else
        return 0;
    }
  return -1;
}

void b_make_move(const move *m) {
  /* is it a short castling for white? */
  if (m->from == 3 && m->to == 1 && (castle[ply_counter] & CASTLE_W_OO)) {
    board[2] = board[0];
    board[0] = 0;
    board[2]->location = 2;
  }
  /* is it a long castling for white? */
  else if (m->from == 3 && m->to == 5 && (castle[ply_counter] & CASTLE_W_OOO)) {
    board[4] = board[7];
    board[7] = 0;
    board[4]->location = 4;
  }
  /* is it a short castling for black? */
  else if (m->from == 115 && m->to == 113 &&
           (castle[ply_counter] & CASTLE_B_OO)) {
    board[114] = board[112];
    board[112] = 0;
    board[114]->location = 114;
  }
  /* is it a long castling for black? */
  else if (m->from == 115 && m->to == 117 &&
           (castle[ply_counter] & CASTLE_B_OOO)) {
    board[116] = board[119];
    board[119] = 0;
    board[116]->location = 116;
  }
  /* is it an en-passant capture? */
  if (m->to == en_passant[ply_counter] &&
      board[m->from]->type == (PAWN | turn)) {
    if (turn & BLACK) {
      trash[ply_counter] = board[m->to + 16];
      board[m->to + 16] = 0;
    } else {
      trash[ply_counter] = board[m->to - 16];
      board[m->to - 16] = 0;
    }
  } else {
    trash[ply_counter] = board[m->to];
  }
  if (trash[ply_counter]) {
    trash[ply_counter]->location = -1;
  }
  // make the move
  board[m->to] = board[m->from];
  board[m->from] = 0;
  board[m->to]->location = m->to;
  // promote if necessary
  if (m->promotion != NONE) {
    board[m->to]->type &= BLACKORWHITE;
    board[m->to]->type |= (m->promotion & PIECE);
  }
  // update game history
  history[ply_counter] = *m;
  ply_counter++;
  // set next en-passant position if necessary
  if (board[m->to]->type == (PAWN | turn)) {
    if (m->to - m->from == 32)
      en_passant[ply_counter] = m->from + 16;
    else if (m->to - m->from == -32)
      en_passant[ply_counter] = m->from - 16;
    else
      en_passant[ply_counter] = NONE;
  } else
    en_passant[ply_counter] = NONE;
  // reset 50-moves counter and castling info
  if (board[m->to]->type == PAWN || trash[ply_counter])
    count_50[ply_counter] = 0;
  else
    count_50[ply_counter] = count_50[ply_counter - 1] + 1;
  castle[ply_counter] =
      castle[ply_counter - 1] & castle_mask[m->from] & castle_mask[m->to];
  // change turn color
  turn ^= BLACKORWHITE;
}

void b_unmake_move() {
  // just decrease counter if greater than zero
  if (ply_counter-- == 0) {
    ply_counter++;
    return;
  }
  // change turn
  turn ^= BLACKORWHITE;
  move *m = &history[ply_counter];
  // undo promotion (put a pawn back)
  if (m->promotion != NONE) {
    board[m->to]->type &= BLACKORWHITE;
    board[m->to]->type |= PAWN;
  }
  // make backwards moves
  board[m->from] = board[m->to];
  board[m->from]->location = m->from;
  if (m->to == en_passant[ply_counter] &&
      board[m->from]->type == (PAWN | turn)) {
    if (turn & BLACK) {
      board[m->to + 16] = trash[ply_counter];
      if (board[m->to + 16])
        board[m->to + 16]->location = m->to + 16;
    } else {
      board[m->to - 16] = trash[ply_counter];
      if (board[m->to - 16])
        board[m->to - 16]->location = m->to - 16;
    }
    board[m->to] = 0;
  } else {
    board[m->to] = trash[ply_counter];
    if (board[m->to])
      board[m->to]->location = m->to;
  }
  /* is it a short white castle? */
  if (m->from == 3 && m->to == 1 && (castle[ply_counter] & CASTLE_W_OO)) {
    board[0] = board[2];
    board[2] = 0;
    board[0]->location = 0;
  }
  /* is it a long white castle? */
  else if (m->from == 3 && m->to == 5 && (castle[ply_counter] & CASTLE_W_OOO)) {
    board[7] = board[4];
    board[4] = 0;
    board[7]->location = 7;
  }
  /* is it a short black castle? */
  else if (m->from == 115 && m->to == 113 &&
           (castle[ply_counter] & CASTLE_B_OO)) {
    board[112] = board[114];
    board[114] = 0;
    board[112]->location = 112;
  }
  /* is it a long black castle? */
  else if (m->from == 115 && m->to == 117 &&
           (castle[ply_counter] & CASTLE_B_OOO)) {
    board[119] = board[116];
    board[116] = 0;
    board[119]->location = 119;
  }
}

void b_reset_board() {
  int i;
  turn = WHITE;
  ply_counter = 0;
  castle[ply_counter] = CASTLE_W_OO | CASTLE_W_OOO | CASTLE_B_OO | CASTLE_B_OOO;
  for (i = 0; i < MAX_PLIES; i++)
    en_passant[i] = NONE;
  wpieces[0].type = PAWN | WHITE;
  wpieces[0].location = 16;
  wpieces[1].type = PAWN | WHITE;
  wpieces[1].location = 17;
  wpieces[2].type = PAWN | WHITE;
  wpieces[2].location = 18;
  wpieces[3].type = PAWN | WHITE;
  wpieces[3].location = 19;
  wpieces[4].type = PAWN | WHITE;
  wpieces[4].location = 20;
  wpieces[5].type = PAWN | WHITE;
  wpieces[5].location = 21;
  wpieces[6].type = PAWN | WHITE;
  wpieces[6].location = 22;
  wpieces[7].type = PAWN | WHITE;
  wpieces[7].location = 23;
  wpieces[8].type = ROOK | WHITE;
  wpieces[8].location = 0;
  wpieces[9].type = ROOK | WHITE;
  wpieces[9].location = 7;
  wpieces[10].type = KNIGHT | WHITE;
  wpieces[10].location = 1;
  wpieces[11].type = KNIGHT | WHITE;
  wpieces[11].location = 6;
  wpieces[12].type = BISHOP | WHITE;
  wpieces[12].location = 2;
  wpieces[13].type = BISHOP | WHITE;
  wpieces[13].location = 5;
  wpieces[14].type = QUEEN | WHITE;
  wpieces[14].location = 4;
  wpieces[15].type = KING | WHITE;
  wpieces[15].location = 3;
  bpieces[0].type = PAWN | BLACK;
  bpieces[0].location = 96;
  bpieces[1].type = PAWN | BLACK;
  bpieces[1].location = 97;
  bpieces[2].type = PAWN | BLACK;
  bpieces[2].location = 98;
  bpieces[3].type = PAWN | BLACK;
  bpieces[3].location = 99;
  bpieces[4].type = PAWN | BLACK;
  bpieces[4].location = 100;
  bpieces[5].type = PAWN | BLACK;
  bpieces[5].location = 101;
  bpieces[6].type = PAWN | BLACK;
  bpieces[6].location = 102;
  bpieces[7].type = PAWN | BLACK;
  bpieces[7].location = 103;
  bpieces[8].type = ROOK | BLACK;
  bpieces[8].location = 112;
  bpieces[9].type = ROOK | BLACK;
  bpieces[9].location = 119;
  bpieces[10].type = KNIGHT | BLACK;
  bpieces[10].location = 113;
  bpieces[11].type = KNIGHT | BLACK;
  bpieces[11].location = 118;
  bpieces[12].type = BISHOP | BLACK;
  bpieces[12].location = 114;
  bpieces[13].type = BISHOP | BLACK;
  bpieces[13].location = 117;
  bpieces[14].type = QUEEN | BLACK;
  bpieces[14].location = 116;
  bpieces[15].type = KING | BLACK;
  bpieces[15].location = 115;
  for (i = 0; i < 128; i++)
    board[i] = 0;
  for (i = 0; i < 16; i++) {
    board[wpieces[i].location] = &wpieces[i];
    board[bpieces[i].location] = &bpieces[i];
  }
}

void b_print_board() {
  int i;
  if (turn & WHITE)
    printf("\n        White ");
  else
    printf("\n        Black ");
  if (castle[ply_counter] & CASTLE_W_OO)
    printf("K");
  if (castle[ply_counter] & CASTLE_W_OOO)
    printf("Q");
  if (castle[ply_counter] & CASTLE_B_OO)
    printf("k");
  if (castle[ply_counter] & CASTLE_B_OOO)
    printf("q");
  if (en_passant[ply_counter] != NONE)
    printf(" %c%d", 'a' + 7 - (en_passant[ply_counter] % 16),
           (int)(en_passant[ply_counter] / 16) + 1);
  printf("\n");
  for (i = 127; i >= 0; i--) {
    if ((i + 1) % 16 == 0)
      printf("\n    %d   ", (i + 1) / 16);
    if (i & 0x88)
      continue;
    if (board[i] == 0) {
      printf(". ");
      continue;
    }
    if (board[i]->location != i)
      printf("ERROR! -> ");
    if (board[i]->type == 33) {
      printf("P ");
      continue;
    }
    if (board[i]->type == 50) {
      printf("R ");
      continue;
    }
    if (board[i]->type == 40) {
      printf("N ");
      continue;
    }
    if (board[i]->type == 52) {
      printf("B ");
      continue;
    }
    if (board[i]->type == 54) {
      printf("Q ");
      continue;
    }
    if (board[i]->type == 38) {
      printf("K ");
      continue;
    }
    if (board[i]->type == 65) {
      printf("p ");
      continue;
    }
    if (board[i]->type == 82) {
      printf("r ");
      continue;
    }
    if (board[i]->type == 72) {
      printf("n ");
      continue;
    }
    if (board[i]->type == 84) {
      printf("b ");
      continue;
    }
    if (board[i]->type == 86) {
      printf("q ");
      continue;
    }
    if (board[i]->type == 70) {
      printf("k ");
      continue;
    }
    printf("%d <-ERROR! ", board[i]->type);
  }
  printf("\n\n\ta b c d e f g h\n\n");
}

int b_get_turn() { return turn; }

int b_get_ply_counter() { return ply_counter; }

int b_get_fifty_counter() { return count_50[ply_counter]; }

const int w_pawn_pos[128] = {
    0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0,   0,   0,
    -40, -40, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1, 2, 3, -10, -10, 3,
    2,   1,   0,  0,  0,  0,  0,  0,  0,  0,  2,  4,  6, 8, 8, 6, 4,   2,   0,
    0,   0,   0,  0,  0,  0,  0,  3,  6,  9,  12, 12, 9, 6, 3, 0, 0,   0,   0,
    0,   0,   0,  0,  4,  8,  12, 16, 16, 12, 8,  4,  0, 0, 0, 0, 0,   0,   0,
    0,   5,   10, 15, 20, 20, 15, 10, 5,  0,  0,  0,  0, 0, 0, 0, 0,   0,   0,
    0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0};

const int w_rook_pos[128] = {
    0, 0,  10, 10, 10, 10, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 30, 30, 30, 30, 30, 30, 30, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0};

const int w_knight_pos[128] = {
    -10, -30, -10, -10, -10, -10, -30, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, -10, -10, -10, -10, -10, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0};

const int w_bishop_pos[128] = {
    -10, -10, -20, -10, -10, -20, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, -10, -10, -10, -10, -10, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0};

const int b_pawn_pos[128] = {
    0,  0,  0,  0,  0,   0,   0, 0,   0,   0, 0, 0, 0, 0,  0,  0,  5,  10, 15,
    20, 20, 15, 10, 5,   0,   0, 0,   0,   0, 0, 0, 0, 4,  8,  12, 16, 16, 12,
    8,  4,  0,  0,  0,   0,   0, 0,   0,   0, 3, 6, 9, 12, 12, 9,  6,  3,  0,
    0,  0,  0,  0,  0,   0,   0, 2,   4,   6, 8, 8, 6, 4,  2,  0,  0,  0,  0,
    0,  0,  0,  0,  1,   2,   3, -10, -10, 3, 2, 1, 0, 0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  -40, -40, 0, 0,   0,   0, 0, 0, 0, 0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,   0,   0, 0,   0,   0, 0, 0, 0, 0};

const int b_rook_pos[128] = {
    0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 30, 30,
    30, 30, 30, 30, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    10, 10, 10, 10, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0};

const int b_knight_pos[128] = {
    -10, -10, -10, -10, -10, -10, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, -30, -10, -10, -10, -10, -30, -10, 0, 0, 0, 0, 0, 0, 0, 0};

const int b_bishop_pos[128] = {
    -10, -10, -10, -10, -10, -10, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   10,  10,  5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   5,   5,   5,   5,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, -10, -20, -10, -10, -20, -10, -10, 0, 0, 0, 0, 0, 0, 0, 0};

int b_evaluate() {
  int wscore = 0, bscore = 0, i;
  for (i = 0; i < 16; i++) {
    if (wpieces[i].location != -1) {
      switch (wpieces[i].type & PIECE) {
      case PAWN:
        wscore += 100 + w_pawn_pos[wpieces[i].location];
        break;
      case KNIGHT:
        wscore += 300 + w_knight_pos[wpieces[i].location];
        break;
      case BISHOP:
        wscore += 325 + w_bishop_pos[wpieces[i].location];
        break;
      case ROOK:
        wscore += 500 + w_rook_pos[wpieces[i].location];
        break;
      case QUEEN:
        wscore += 900;
        break;
      case KING:
        wscore += 20000;
        break;
      }
    }
    if (bpieces[i].location != -1) {
      switch (bpieces[i].type & PIECE) {
      case PAWN:
        bscore += 100 + b_pawn_pos[bpieces[i].location];
        break;
      case KNIGHT:
        bscore += 300 + b_knight_pos[bpieces[i].location];
        break;
      case BISHOP:
        bscore += 325 + b_bishop_pos[bpieces[i].location];
        break;
      case ROOK:
        bscore += 500 + b_rook_pos[bpieces[i].location];
        break;
      case QUEEN:
        bscore += 900;
        break;
      case KING:
        bscore += 20000;
        break;
      }
    }
  }
  if (turn == WHITE)
    return wscore - bscore;
  else
    return bscore - wscore;
}

void b_sort_moves(move_set *ms, int flags) {
  if (flags & FLAG_EVAL) {
    int i;
    move *m = s_get_killer(ply_counter);
    for (i = 0; i < ms->nmoves; i++) {
      if (ms->move[i].from == m->from && ms->move[i].to == m->to) {
        ms->move[i].value = 1000;
      } else if (ms->move[i].promotion != NONE) {
        switch (ms->move[i].promotion) {
        case ROOK:
          ms->move[i].value += 1501;
          break;
        case KNIGHT:
          ms->move[i].value += 1301;
          break;
        case BISHOP:
          ms->move[i].value += 1326;
          break;
        case QUEEN:
          ms->move[i].value += 1901;
          break;
        }
      } else if (board[ms->move[i].to]) {
        switch (board[ms->move[i].to]->type & PIECE) {
        case PAWN:
          ms->move[i].value = 1101;
          break;
        case ROOK:
          ms->move[i].value = 1501;
          break;
        case KNIGHT:
          ms->move[i].value = 1301;
          break;
        case BISHOP:
          ms->move[i].value = 1326;
          break;
        case QUEEN:
          ms->move[i].value = 1901;
          break;
        }
        switch (board[ms->move[i].from]->type & PIECE) {
        case PAWN:
          ms->move[i].value -= 100;
          break;
        case ROOK:
          ms->move[i].value -= 500;
          break;
        case KNIGHT:
          ms->move[i].value -= 300;
          break;
        case BISHOP:
          ms->move[i].value -= 325;
          break;
        case QUEEN:
          ms->move[i].value -= 900;
          break;
        }
      }
    }
  }
  if (flags & FLAG_ALL)
    u_headsort(ms, ms->nmoves - 1, ms->nmoves);
  else
    u_headsort(ms, ms->nmoves - 1, HEAD);
}
