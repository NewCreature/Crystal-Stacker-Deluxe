#include "t3f/t3f.h"
#include "t3f/controller.h"
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
T3F_CONTROLLER * csd_controller[CSD_MAX_PLAYERS] = {NULL};
int csd_controller_repeat[CSD_MAX_PLAYERS][5] = {{0}};

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

void csd_set_default_controls(int controller)
{
	int default_key[CSD_MAX_PLAYERS][5] = 
	{
		{ ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_UP, ALLEGRO_KEY_PAD_0 },
		{ ALLEGRO_KEY_A,    ALLEGRO_KEY_D,     ALLEGRO_KEY_S,    ALLEGRO_KEY_W,  ALLEGRO_KEY_E },
		{ ALLEGRO_KEY_F,    ALLEGRO_KEY_H,     ALLEGRO_KEY_G,    ALLEGRO_KEY_T,  ALLEGRO_KEY_Y },
		{ ALLEGRO_KEY_J,    ALLEGRO_KEY_L,     ALLEGRO_KEY_K,    ALLEGRO_KEY_I,  ALLEGRO_KEY_O }
	};
	int i;
	
	for(i = 0; i < 5; i++)
	{
		csd_controller[controller]->binding[i].type = T3F_CONTROLLER_BINDING_KEY;
		csd_controller[controller]->binding[i].button = default_key[controller][i];
	}
}

void csd_setup_controllers(void)
{
	char section_name[256] = {0};
	int i;
	
	for(i = 0; i < CSD_MAX_PLAYERS; i++)
	{
		csd_controller[i] = t3f_create_controller(5);
		sprintf(section_name, "Controller %d", i);
		if(!t3f_read_controller_config(t3f_config, section_name, csd_controller[i]))
		{
			csd_set_default_controls(i);
		}
	}
}

bool csd_load_images(void)
{
	csd_bitmap[CSD_BITMAP_TITLE_BG] = al_load_bitmap("graphics/title_bg.png");
	if(!csd_bitmap[CSD_BITMAP_TITLE_BG])
	{
		return false;
	}
	csd_bitmap[CSD_BITMAP_TITLE_LOGO] = al_load_bitmap("graphics/title_logo.png");
	if(!csd_bitmap[CSD_BITMAP_TITLE_LOGO])
	{
		return false;
	}
	return true;
}

bool csd_initialize(int argc, char * argv[])
{
	if(!t3f_initialize("Crystal Stacker Deluxe", 640, 480, 60.0, csd_logic, csd_render, T3F_DEFAULT | T3F_USE_MOUSE | T3F_RESIZABLE | T3F_FORCE_ASPECT))
	{
		return false;
	}
	al_init_ttf_addon();
	if(!csd_load_images())
	{
		return false;
	}
	csd_read_config();
	csd_setup_controllers();
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
