#ifndef CSD_CORE_H
#define CSD_CORE_H

#include "player.h"

#define BLOCKS_PER_LEVEL 45

#define CSD_BLOCK_MAX_TYPES 16

#define CSD_BLOCK_TYPES      10
#define CSD_BLOCK_NTYPES      5
#define CSD_BLOCK_TYPE_WILD  13
#define CSD_BLOCK_TYPE_BOMB  14
#define CSD_BLOCK_TYPE_SOLID 15

/* stuff used by outside modules */
void csd_core_delete_block(CSD_PLAYER_BOARD * bp, int boardx, int boardy);
void csd_core_insert_solid_row(CSD_PLAYER_BOARD * bp);
int csd_core_find_runs(CSD_PLAYER_BOARD * bp, int bx, int by, int dx, int dy);
int csd_core_find_all_runs(CSD_PLAYER_BOARD * bp);
void csd_core_update_level(CSD_PLAYER_BOARD * bp);
int core_delete_runs(CSD_PLAYER_BOARD * bp);
void core_mark_runs_bomb(CSD_PLAYER_BOARD * bp, int x, int y);
int core_find_all_runs(CSD_PLAYER_BOARD * bp);

#endif
