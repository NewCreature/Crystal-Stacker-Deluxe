#ifndef CSD_THEME_H
#define CSD_THEME_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "t3f/animation.h"
#include "core.h"

#define CSD_THEME_STAGES           20
#define CSD_THEME_ANIMATIONS       32
#define CSD_THEME_SAMPLES          32

#define CSD_MAX_PLAYERS             4

#define CSD_THEME_ANIMATION_BACKGROUND 0
#define CSD_THEME_ANIMATION_PLAYGROUND 1
#define CSD_THEME_ANIMATION_CRYSTALS   2

#define CSD_THEME_FLASH_OVERLAY        0

#define CSD_THEME_SAMPLE_DROP          0
#define CSD_THEME_SAMPLE_ROTATE_UP     1
#define CSD_THEME_SAMPLE_ROTATE_DOWN   2
#define CSD_THEME_SAMPLE_MOVE          3
#define CSD_THEME_SAMPLE_MOVE_FAIL     4
#define CSD_THEME_SAMPLE_LAND          5
#define CSD_THEME_SAMPLE_MATCHES       6
#define CSD_THEME_SAMPLE_DELETE        7
#define CSD_THEME_SAMPLE_LEVEL         8
#define CSD_THEME_SAMPLE_PAUSE         9
#define CSD_THEME_SAMPLE_ATTACK       10


typedef struct
{
	
	T3F_ANIMATION * animation[CSD_THEME_ANIMATIONS];
	ALLEGRO_FONT * font;
	ALLEGRO_SAMPLE * sample[CSD_THEME_SAMPLES];
	float playground_x[CSD_MAX_PLAYERS];
	float playground_y[CSD_MAX_PLAYERS];
	float preview_x[CSD_MAX_PLAYERS];
	float preview_y[CSD_MAX_PLAYERS];
	int block_width;
	int block_height;
	int board_width;
	int board_height;
	int stack_height;
	int block_types;
	int flash_type;
	
	T3F_ANIMATION * crystal_animation[CSD_BLOCK_MAX_TYPES];
	T3F_ANIMATION * fcrystal_animation[CSD_BLOCK_MAX_TYPES];
	
} CSD_STAGE;

typedef struct
{
	
	/* path theme was loaded from */
	ALLEGRO_PATH * path;

	/* this configuration holds information about the theme */
	ALLEGRO_CONFIG * config;
	
	int max_players;
	
} CSD_THEME;

CSD_THEME * csd_load_theme(const char * fn);
void csd_destroy_theme(CSD_THEME * tp);
bool csd_should_load_stage(CSD_THEME * tp, int stage);
CSD_STAGE * csd_load_stage(CSD_THEME * tp, int stage);
void csd_destroy_stage(CSD_STAGE * sp);

#endif
