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
	
} CSD_GAME;

void csd_game_init_level(CSD_GAME * gp, int level, int player);
void csd_game_logic(CSD_GAME * gp);
void csd_game_render(CSD_GAME * gp);

#endif
