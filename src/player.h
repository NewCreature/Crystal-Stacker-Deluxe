#ifndef PLAYER_H
#define PLAYER_H

#include "message.h"

/* array maximums */
#define CSD_PLAYER_BOARD_W                32
#define CSD_PLAYER_BOARD_H                32
#define CSD_PLAYER_BLOCK_H                 3

/* board states */
#define CSD_PLAYER_BOARD_STATE_NORMAL      0
#define CSD_PLAYER_BOARD_STATE_RUNS        1

/* block states */
#define CSD_PLAYER_BLOCK_STATE_ON_BOARD    0
#define CSD_PLAYER_BLOCK_STATE_TO_BOARD    1
#define CSD_PLAYER_BLOCK_STATE_ON_PREVIEW  2
#define CSD_PLAYER_BLOCK_STATE_TO_PREVIEW  3

typedef struct
{
	
	char data[CSD_PLAYER_BLOCK_H];
	
	/* fixed point position and velocity used internally to control movement speed */
	float x, y, vx, vy;
	
	/* on-screen coordinates in block mode */
	int bx, by;
	
	int state;
	int type;
	
	int id;
	
} CSD_PLAYER_BLOCK;

typedef struct
{
	
	char data[CSD_PLAYER_BOARD_H][CSD_PLAYER_BOARD_W];
	char flag[CSD_PLAYER_BOARD_H][CSD_PLAYER_BOARD_W];
	int width, height;
	int state;
	int state_counter;
	
	int id;
	
} CSD_PLAYER_BOARD;

typedef struct
{
	
	CSD_PLAYER_BLOCK block;
	CSD_PLAYER_BLOCK block_preview;
	CSD_PLAYER_BOARD board;
	CSD_MESSAGE_QUEUE messages;
	
	int score;
	int destroyed;
	int level;
	int profile;
	int active;
	int high_combo;
	bool lost;
	
	/* internally used */
	int combo, removed;
	
	int id;
	
} CSD_PLAYER;

void csd_player_clear_board(CSD_PLAYER * pp);
void csd_player_block_generate(CSD_PLAYER_BLOCK * bp);
void csd_player_block_copy(CSD_PLAYER_BLOCK * dest, CSD_PLAYER_BLOCK * source);
bool csd_player_board_full(CSD_PLAYER * pp);
void csd_player_get_next_block(CSD_PLAYER * pp);
void csd_player_init(CSD_PLAYER * pp, int level);

#endif
