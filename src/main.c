#include "t3f/t3f.h"
#include "t3net/t3net.h"
#include <allegro5/allegro_ttf.h>
#include "main.h"
#include "title.h"
#include "leaderboard.h"
#include "theme.h"
#include "game.h"

/* main data */
int csd_state = CSD_STATE_TITLE;
int csd_option[CSD_MAX_OPTIONS] = {0};
ALLEGRO_BITMAP * csd_bitmap[CSD_MAX_BITMAPS] = {NULL};
int csd_high_score = 1000;
T3NET_LEADERBOARD * csd_leaderboard = NULL;
int csd_leaderboard_position = -1;
char csd_upload_name[256] = {0};

int csd_tick = 0;
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
		case CSD_STATE_LEADERBOARD:
		{
			csd_leaderboard_logic();
			break;
		}
	}
	csd_tick++;
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
			break;
		}
		case CSD_STATE_LEADERBOARD:
		{
			csd_leaderboard_render();
			break;
		}
	}
}

void csd_read_config(void)
{
	const char * val;
	
	/* local high score */
	val = al_get_config_value(t3f_config, "Game Data", "High Score");
	if(val)
	{
		csd_high_score = atoi(val);
	}
	
	/* upload scores */
	csd_option[CSD_OPTION_UPLOAD] = 1;
	val = al_get_config_value(t3f_config, "Options", "Upload Scores");
	if(val)
	{
		if(strcasecmp(val, "true"))
		{
			csd_option[CSD_OPTION_UPLOAD] = 0;
		}
	}

	/* upload name */
	strcpy(csd_upload_name, "Unnamed");
	val = al_get_config_value(t3f_config, "Options", "Upload Name");
	if(val)
	{
		strcpy(csd_upload_name, val);
	}
}

bool csd_initialize(int argc, char * argv[])
{
	if(!t3f_initialize("Crystal Stacker Deluxe", 640, 480, 60.0, csd_logic, csd_render, T3F_DEFAULT | T3F_USE_MOUSE))
	{
		return false;
	}
	al_init_ttf_addon();
	csd_read_config();
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
