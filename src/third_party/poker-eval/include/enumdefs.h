/*
 * Copyright (C) 2002-2006
 *           Michael Maurer <mjmaurer@yahoo.com>
 *           Loic Dachary <loic@dachary.org>
 *
 * This program gives you software freedom; you can copy, convey,
 * propagate, redistribute and/or modify this program under the terms of
 * the GNU General Public License (GPL) as published by the Free Software
 * Foundation (FSF), either version 3 of the License, or (at your option)
 * any later version of the GPL published by the FSF.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program in a file in the toplevel directory called "GPLv3".
 * If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id: enumdefs.h 5146 2008-12-04 03:11:22Z bkuhn $ */

#ifndef ENUMDEFS_H
#define ENUMDEFS_H

#include "enumord.h"
#include "pokereval_export.h"

#define ENUM_MAXPLAYERS 9

typedef enum {
  /* do not reorder -- some other static arrays depend on this order */
  game_holdem,
  game_NUMGAMES
} enum_game_t;

typedef struct {
  enum_game_t	game;
  int           minpocket;
  int           maxpocket;
  int          	maxboard;
  int		haslopot;
  int		hashipot;
  char *	name;
} enum_gameparams_t;

typedef enum { ENUM_EXHAUSTIVE, ENUM_SAMPLE } enum_sample_t;

typedef struct {
  enum_game_t game;
  enum_sample_t sampleType;
  unsigned int nsamples;
  unsigned int nplayers;
  unsigned int nwinhi[ENUM_MAXPLAYERS];	/* qualifies for high and wins (no tie) */
  unsigned int ntiehi[ENUM_MAXPLAYERS];	/* qualifies for high and ties */
  unsigned int nlosehi[ENUM_MAXPLAYERS];/* qualifies for high and loses */
  unsigned int nwinlo[ENUM_MAXPLAYERS];	/* qualifies for low and wins (no tie) */
  unsigned int ntielo[ENUM_MAXPLAYERS];	/* qualifies for low and ties */
  unsigned int nloselo[ENUM_MAXPLAYERS];/* qualifies for low and loses */
  unsigned int nscoop[ENUM_MAXPLAYERS]; /* wins entire pot */

  /* nsharehi[i][H] is the number of times that player i tied for the best
     high hand with H total players (including player i), or received no
     share of the pot if H=0; likewise for nsharelo. */
  unsigned int nsharehi[ENUM_MAXPLAYERS][ENUM_MAXPLAYERS+1];
  unsigned int nsharelo[ENUM_MAXPLAYERS][ENUM_MAXPLAYERS+1];

  /* nshare[i][H][L] is the number of times that player i tied for the best
     high hand with H total players (including player i) and simultaneously
     tied for the best low hand with L total players (including player i),
     where H=0 and L=0 indicate that player i did not win the corresponding
     share of the pot.  For example, nshare[i][1][1] is the number of times
     that player i scooped both high and low; nshare[i][1][2] is the number of
     times that player i won high and split low with one player.  Note that
     the H=0 and L=0 buckets include cases where player i didn't qualify
     (e.g., for an 8-or-better low), which differs from the definition of
     nlosehi[] and nloselo[] above.  So you can't compute ev[] from
     nshare[][][]. */
  unsigned int nshare[ENUM_MAXPLAYERS][ENUM_MAXPLAYERS+1][ENUM_MAXPLAYERS+1];

  /* ev[i] is the pot equity of player i averaged over all outcomes */
  double ev[ENUM_MAXPLAYERS];

  enum_ordering_t *ordering;	/* detailed relative hand rank ordering */
} enum_result_t;  

extern POKEREVAL_EXPORT void enumResultPrint(enum_result_t *result, StdDeck_CardMask pockets[],
                            StdDeck_CardMask board);
extern POKEREVAL_EXPORT void enumResultPrintTerse(enum_result_t *result,
                                 StdDeck_CardMask pockets[],
                                 StdDeck_CardMask board);
extern POKEREVAL_EXPORT void enumResultClear(enum_result_t *result);
extern POKEREVAL_EXPORT void enumResultFree(enum_result_t *result);
extern POKEREVAL_EXPORT int enumResultAlloc(enum_result_t *result, int nplayers,
                           enum_ordering_mode_t mode);
extern POKEREVAL_EXPORT int enumExhaustive(enum_game_t game, StdDeck_CardMask pockets[],
                          StdDeck_CardMask board, StdDeck_CardMask dead,
                          int npockets, int nboard, int orderflag,
                          enum_result_t *result);
extern POKEREVAL_EXPORT int enumSample(enum_game_t game, StdDeck_CardMask pockets[],
                      StdDeck_CardMask board, StdDeck_CardMask dead,
                      int npockets, int nboard, int niter, int orderflag,
                      enum_result_t *result);
extern POKEREVAL_EXPORT enum_gameparams_t *enumGameParams(enum_game_t game);

#endif
