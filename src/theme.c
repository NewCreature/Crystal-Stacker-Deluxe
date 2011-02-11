#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "../../../Dev/T3F/t3f/music.h"
#include <stdio.h>
#include "theme.h"

static char * csd_theme_sample_name[CSD_THEME_SAMPLES] =
{
	"Drop Sound",
	"Rotate Up Sound",
	"Rotate Down Sound",
	"Move Sound",
	"Move Fail Sound",
	"Land Sound",
	"Matches Sound",
	"Delete Sound",
	"Level Sound",
	"Pause Sound",
	"Attack Sound"
};

/* load animation, determining if we need to load an image or animation based on the filename */
static T3F_ANIMATION * csd_load_animation(ALLEGRO_PATH * path)
{
	const char * extension = NULL;
	
	extension = al_get_path_extension(path);
	if(extension)
	{
		if(!strcasecmp(extension, ".ani"))
		{
			return t3f_load_animation(al_path_cstr(path, '/'));
		}
		else
		{
			return t3f_load_animation_from_bitmap(al_path_cstr(path, '/'));
		}
	}
	return NULL;
}

CSD_THEME * csd_load_theme(const char * fn)
{
	CSD_THEME * tp;
	const char * value;
	
	tp = malloc(sizeof(CSD_THEME));
	if(!tp)
	{
		return NULL;
	}
	memset(tp, 0, sizeof(CSD_THEME));
	
	tp->path = al_create_path(fn);
	if(!tp->path)
	{
		return NULL;
	}
	
	tp->config = al_load_config_file(fn);
	if(!tp->config)
	{
		free(tp);
		return NULL;
	}
	value = al_get_config_value(tp->config, "Settings", "Max Players");
	if(!value)
	{
		tp->max_players = 1;
	}
	else
	{
		tp->max_players = atol(value);
	}
	return tp;
}

void csd_destroy_theme(CSD_THEME * tp)
{
	free(tp);
}

/* at level change, check if new stage should be loaded */
bool csd_should_load_stage(CSD_THEME * tp, int stage)
{
	char stage_name[256] = {0};
	const char * value;
	
	sprintf(stage_name, "Stage %d", stage);
	value = al_get_config_value(tp->config, stage_name, "Load");
	if(!value)
	{
		return false;
	}
	return true;
}

/* load the specified stage, should only be called if csd_should_load_stage
   returned true */
CSD_STAGE * csd_load_stage(CSD_THEME * tp, int stage)
{
	CSD_STAGE * sp;
	char stage_name[256] = {0};
	char vname[256] = {0};
	const char * value;
	int fs = 16;
	int i;
	
	sp = malloc(sizeof(CSD_STAGE));
	if(!sp)
	{
		return NULL;
	}
	memset(sp, 0, sizeof(CSD_STAGE));
	
	sprintf(stage_name, "Stage %d", stage);
	
	/* load the font */
	value = al_get_config_value(tp->config, stage_name, "Font");
	if(value)
	{
		al_set_path_filename(tp->path, value);
		value = al_get_config_value(tp->config, stage_name, "Font Size");
		if(value)
		{
			fs = atoi(value);
		}
		sp->font = al_load_font(al_path_cstr(tp->path, '/'), fs, 0);
	}
	
	/* load the background image */
	value = al_get_config_value(tp->config, stage_name, "Background");
	if(!value)
	{
		return NULL;
	}
	al_set_path_filename(tp->path, value);
	printf("%s\n", value);
	sp->animation[CSD_THEME_ANIMATION_BACKGROUND] = csd_load_animation(tp->path);
	
	/* load the playground image */
	value = al_get_config_value(tp->config, stage_name, "Playground");
	if(!value)
	{
		return NULL;
	}
	al_set_path_filename(tp->path, value);
	sp->animation[CSD_THEME_ANIMATION_PLAYGROUND] = csd_load_animation(tp->path);
	
	/* load the playground positions */
	for(i = 0; i < tp->max_players; i++)
	{
		sprintf(vname, "Playground %d X", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->playground_x[i] = atol(value);
		}
		sprintf(vname, "Playground %d Y", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->playground_y[i] = atof(value);
		}
	}
	
	/* load crystal animations */
	for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
	{
		sprintf(vname, "Crystal %d", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			printf("%s\n", value);
			al_set_path_filename(tp->path, value);
			sp->crystal_animation[i] = csd_load_animation(tp->path);
			if(sp->crystal_animation[i])
			{
				sp->crystal_animation[i]->frame[0]->width = 28.0;
				sp->crystal_animation[i]->frame[0]->height = 28.0;
			}
			if(!sp->crystal_animation[i])
			{
				printf("Fuck!\n");
			}
		}
	}
	
	/* load flash crystal animations */
	for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
	{
		sprintf(vname, "Flash Crystal %d", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			printf("%s\n", value);
			al_set_path_filename(tp->path, value);
			sp->fcrystal_animation[i] = csd_load_animation(tp->path);
			if(sp->fcrystal_animation[i])
			{
				sp->fcrystal_animation[i]->frame[0]->width = 28.0;
				sp->fcrystal_animation[i]->frame[0]->height = 28.0;
			}
			if(!sp->fcrystal_animation[i])
			{
				printf("Fuck!\n");
			}
		}
	}
	
	/* load sounds */
	for(i = 0; i < CSD_THEME_SAMPLES; i++)
	{
		if(csd_theme_sample_name[i])
		{
			sprintf(vname, "%s", csd_theme_sample_name[i]);
			value = al_get_config_value(tp->config, stage_name, vname);
			if(value)
			{
				printf("%s\n", value);
				al_set_path_filename(tp->path, value);
				sp->sample[i] = al_load_sample(al_path_cstr(tp->path, '/'));
				if(sp->sample[i])
				{
					printf("loaded!\n");
				}
			}
		}
	}
	
	value = al_get_config_value(tp->config, stage_name, "Board Width");
	if(value)
	{
		sp->board_width = atol(value);;
	}
	else
	{
		sp->board_width = 6;
	}
	value = al_get_config_value(tp->config, stage_name, "Board Height");
	if(value)
	{
		sp->board_height = atol(value);;
	}
	else
	{
		sp->board_height = 14;
	}
	value = al_get_config_value(tp->config, stage_name, "Stack Height");
	if(value)
	{
		sp->stack_height = atol(value);;
	}
	else
	{
		sp->stack_height = 3;
	}
	
	value = al_get_config_value(tp->config, stage_name, "Flash Type");
	if(value)
	{
		if(!strcmp(value, "Overlay"))
		{
			sp->flash_type = CSD_THEME_FLASH_OVERLAY;
		}
	}
	else
	{
		sp->flash_type = CSD_THEME_FLASH_OVERLAY;
	}
	
	/* play music */
	value = al_get_config_value(tp->config, stage_name, "Music");
	if(!value)
	{
		return NULL;
	}
	al_set_path_filename(tp->path, value);
	t3f_play_music(al_path_cstr(tp->path, '/'));
//	dumba5_load_and_play_module(value, 0, true, 44100, true);	
	
	return sp;
}

void csd_destroy_stage(CSD_STAGE * sp)
{
	free(sp);
}
