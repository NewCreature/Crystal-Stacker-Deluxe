#include "t3f/t3f.h"
#include <allegro5/allegro_ttf.h>
#include "main.h"
#include "theme.h"
#include "game.h"

int csd_state = CSD_STATE_PLAYING;
ALLEGRO_BITMAP * csd_bitmap[CSD_MAX_BITMAPS] = {NULL};

CSD_THEME * csd_theme = NULL;
CSD_STAGE * csd_stage = NULL;
CSD_GAME csd_game;

int level_chart[20] = {670, 600, 540, 480, 420, 370, 320, 280, 240, 200, 170, 140, 120, 100, 80, 70, 60, 55, 50, 45};
int level = 0;
float ratio;
int frame = 0;
int done = 0;

void csd_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || done)
	{
		t3f_exit();
	}
	
	switch(csd_state)
	{
		case CSD_STATE_PLAYING:
		{
			csd_game_logic(&csd_game);
			break;
		}
	}
}

void csd_render(void)
{
	switch(csd_state)
	{
		case CSD_STATE_PLAYING:
		{
			csd_game_render(&csd_game);
/*			draw_animation(csd_stage->animation[CSD_THEME_ANIMATION_BACKGROUND], 0, 0.0, 0.0, 0.0, 0);
			draw_animation(csd_stage->animation[CSD_THEME_ANIMATION_PLAYGROUND], 0, csd_stage->playground_x[0], csd_stage->playground_y[0], 0.0, 0);
			for(i = 0; i < 14; i++)
			{
				for(j = 0; j < 6; j++)
				{
					draw_animation(csd_stage->crystal_animation[rand() % 5], 0, j * 32, i * 32, 0.0, 0);
				}
			} */
			break;
		}
	}
}

bool csd_initialize(int argc, char * argv[])
{
	int i, j;
	
	if(!t3f_initialize("Crystal Stacker Deluxe", 640, 480, 60.0, csd_logic, csd_render, T3F_DEFAULT))
	{
		return false;
	}
	al_init_ttf_addon();
	
	memset(&csd_game, 0, sizeof(CSD_GAME));
	csd_theme = csd_load_theme("themes/default/theme.ini");
	if(!csd_theme)
	{
		printf("Error loading theme!\n");
		return false;
	}
	csd_game.theme = csd_theme;
	csd_game_init_level(&csd_game, 0, 0);
	for(i = 0; i < csd_game.stage.board_height; i++)
	{
		for(j = 0; j < csd_game.stage.board_width; j++)
		{
			csd_game.player[0].board.data[i][j] = 0;
			csd_game.player[0].board.flag[i][j] = 0;
		}
	}
	csd_game.players = 1;
	for(i = 0; i < csd_game.players; i++)
	{
		csd_game.player[i].block.x = (csd_game.stage.board_width / 2) * csd_game.stage.crystal_animation[0]->frame[0]->width;
		csd_game.player[i].block.y = 0.0;
		csd_player_block_generate(&csd_game.player[i].block);
		csd_player_block_generate(&csd_game.player[i].block_preview);
	}
	csd_game.state = CSD_GAME_STATE_PLAY;
	return true;
}

void csd_exit(void)
{
}

int main(int argc, char * argv[])
{
	if(!csd_initialize(argc, argv))
	{
		return -1;
	}
	t3f_run();
	return 0;
}
