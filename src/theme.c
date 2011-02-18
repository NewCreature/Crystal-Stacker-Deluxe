#include "t3f/t3f.h"
#include "t3f/music.h"
#include <stdio.h>
#include "theme.h"
#include "legacy_theme.h"

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
		if(!strcasecmp(extension, ".t3a"))
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
	ALLEGRO_PATH * path;
	
	path = al_create_path(fn);
	if(path)
	{
		value = al_get_path_extension(path);
		if(value)
		{
			if(!strcasecmp(value, ".cth"))
			{
				al_destroy_path(path);
				return csd_load_legacy_theme(fn);
			}
		}
		al_destroy_path(path);
	}
	
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

/* load the specified stage, should only be called if csd_should_load_stage
   returned true */
bool csd_load_stage(CSD_THEME * tp, CSD_STAGE * sp, int stage)
{
	char stage_name[256] = {0};
	char vname[256] = {0};
	const char * value;
	int fs = 16;
	bool reatlas = false;
	int i, j;
	
	value = al_get_config_value(tp->config, "Settings", "Legacy");
	if(value)
	{
		if(stage == 0)
		{
			return csd_load_legacy_stage(tp, sp);
		}
		return true;
	}
	sprintf(stage_name, "Stage %d", stage);
	
	/* clear stage data if we are loading the first stage so we don't get junk */
	if(stage == 0)
	{
		memset(sp, 0, sizeof(CSD_STAGE));
		sp->atlas = t3f_create_atlas(T3F_ATLAS_SPRITES, 1024, 1024);
	}
	
	/* play music first so we can here it while things load */
	value = al_get_config_value(tp->config, stage_name, "Music");
	if(value)
	{
		al_set_path_filename(tp->path, value);
		t3f_play_music(al_path_cstr(tp->path, '/'));
	}
	
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
	if(value)
	{
		al_set_path_filename(tp->path, value);
		printf("%s\n", value);
		sp->animation[CSD_THEME_ANIMATION_BACKGROUND] = csd_load_animation(tp->path);
	}
	
	/* load the playground image */
	value = al_get_config_value(tp->config, stage_name, "Playground");
	if(value)
	{
		al_set_path_filename(tp->path, value);
		sp->animation[CSD_THEME_ANIMATION_PLAYGROUND] = csd_load_animation(tp->path);
	}
	
	/* block size */
	value = al_get_config_value(tp->config, stage_name, "Block Width");
	if(value)
	{
		sp->block_width = atoi(value);
	}
	value = al_get_config_value(tp->config, stage_name, "Block Height");
	if(value)
	{
		sp->block_height = atoi(value);
	}
	value = al_get_config_value(tp->config, stage_name, "Block Scale");
	if(value)
	{
		if(!strcasecmp(value, "true"))
		{
			sp->block_scale = true;
		}
	}
	
	/* load the playground positions */
	for(i = 0; i < tp->max_players; i++)
	{
		sprintf(vname, "Playground %d X", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->layout[i].playground.x = atof(value);
		}
		sprintf(vname, "Playground %d Y", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->layout[i].playground.y = atof(value);
		}
	}
	
	/* load the message positions */
	for(i = 0; i < tp->max_players; i++)
	{
		sprintf(vname, "Message %d X", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->layout[i].message.x = atof(value);
		}
		sprintf(vname, "Message %d Y", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->layout[i].message.y = atof(value);
		}
		sprintf(vname, "Message %d Width", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->message_width = atof(value);
		}
		sprintf(vname, "Message %d Height", i);
		value = al_get_config_value(tp->config, stage_name, vname);
		if(value)
		{
			sp->message_height = atof(value);
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
				if(sp->block_scale)
				{
					for(j = 0; j < sp->crystal_animation[i]->frames; j++)
					{
						sp->crystal_animation[i]->frame[j]->width = sp->block_width;
						sp->crystal_animation[i]->frame[j]->height = sp->block_height;
					}
				}
			}
			if(!sp->crystal_animation[i])
			{
				printf("Fuck!\n");
			}
			reatlas = true;
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
				if(sp->block_scale)
				{
					for(j = 0; j < sp->fcrystal_animation[i]->frames; j++)
					{
						sp->fcrystal_animation[i]->frame[j]->width = 28.0;
						sp->fcrystal_animation[i]->frame[j]->height = 28.0;
					}
				}
			}
			if(!sp->fcrystal_animation[i])
			{
				printf("Fuck!\n");
			}
			reatlas = true;
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
				al_set_path_filename(tp->path, value);
				sp->sample[i] = al_load_sample(al_path_cstr(tp->path, '/'));
			}
		}
	}
	
/*	value = al_get_config_value(tp->config, stage_name, "Board Width");
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
	} */
	sp->board_width = 6;
	sp->board_height = 14;
	
/*	value = al_get_config_value(tp->config, stage_name, "Stack Height");
	if(value)
	{
		sp->stack_height = atol(value);;
	}
	else
	{
		sp->stack_height = 3;
	} */
	sp->stack_height = 3;
	
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
	
	if(reatlas)
	{
		if(sp->atlas)
		{
			t3f_destroy_atlas(sp->atlas);
		}
		sp->atlas = t3f_create_atlas(T3F_ATLAS_SPRITES, 1024, 1024);
		if(sp->atlas)
		{
			for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
			{
				if(sp->crystal_animation[i])
				{
					t3f_add_animation_to_atlas(sp->atlas, sp->crystal_animation[i]);
				}
			}
			for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
			{
				if(sp->fcrystal_animation[i])
				{
					t3f_add_animation_to_atlas(sp->atlas, sp->fcrystal_animation[i]);
				}
			}
		}
	}
	
	return true;
}

void csd_free_stage(CSD_STAGE * sp)
{
	int i;
	
	for(i = 0; i < CSD_THEME_ANIMATIONS; i++)
	{
		if(sp->animation[i])
		{
			t3f_destroy_animation(sp->animation[i]);
			sp->animation[i] = NULL;
		}
	}
	if(sp->font)
	{
		al_destroy_font(sp->font);
		sp->font = NULL;
	}
	for(i = 0; i < CSD_THEME_SAMPLES; i++)
	{
		if(sp->sample[i])
		{
			al_destroy_sample(sp->sample[i]);
			sp->sample[i] = NULL;
		}
	}
	for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
	{
		if(sp->crystal_animation[i])
		{
			t3f_destroy_animation(sp->crystal_animation[i]);
			sp->crystal_animation[i] = NULL;
		}
	}
	for(i = 0; i < CSD_BLOCK_MAX_TYPES; i++)
	{
		if(sp->fcrystal_animation[i])
		{
			t3f_destroy_animation(sp->fcrystal_animation[i]);
			sp->fcrystal_animation[i] = NULL;
		}
	}
	if(sp->atlas)
	{
		t3f_destroy_atlas(sp->atlas);
	}
}
