#include "t3f/t3f.h"
#include <allegro5/allegro_ttf.h>
#include "main.h"
#include "title.h"
#include "theme.h"
#include "game.h"

/* main data */
int csd_state = CSD_STATE_TITLE;
ALLEGRO_BITMAP * csd_bitmap[CSD_MAX_BITMAPS] = {NULL};

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
		case CSD_STATE_TITLE:
		{
			csd_title_logic();
			break;
		}
		case CSD_STATE_PLAYING:
		{
			csd_game_logic();
			break;
		}
	}
}

void csd_render(void)
{
	switch(csd_state)
	{
		case CSD_STATE_TITLE:
		{
			csd_title_render();
			break;
		}
		case CSD_STATE_PLAYING:
		{
			csd_game_render();
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
	if(!t3f_initialize("Crystal Stacker Deluxe", 640, 480, 60.0, csd_logic, csd_render, T3F_DEFAULT | T3F_USE_MOUSE))
	{
		return false;
	}
	al_init_ttf_addon();
	if(!csd_title_setup())
	{
		return false;
	}
	srand(time(0));
	return true;
}

void csd_exit(void)
{
	t3f_exit();
}

void csd_set_state(int state)
{
	csd_state = state;
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
