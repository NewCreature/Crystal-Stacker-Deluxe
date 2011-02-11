#ifndef GAME_H
#define GAME_H

#include "theme.h"
#include "player.h"
#include "3dsprite.h"

#define CSD_MAX_PLAYERS 4

#define CSD_GAME_STATE_INTRO 0
#define CSD_GAME_STATE_PLAY  1
#define CSD_GAME_STATE_PAUSE 2
#define CSD_GAME_STATE_OVER  3

typedef struct
{
	
	CSD_THEME * theme;
	CSD_STAGE stage;
	CSD_PLAYER player[CSD_MAX_PLAYERS];
	CSD_SPRITE_3D sprite[128];
	
	int players;
	
	int state;
	int tick;
	
} CSD_GAME;

bool csd_game_setup(void);
void csd_game_exit(void);
void csd_game_init_level(int level, int player);
void csd_game_logic(void);
void csd_game_render(void);

#endif
