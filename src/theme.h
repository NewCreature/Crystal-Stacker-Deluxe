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

#define CSD_THEME_ANIMATION_BACKGROUND 0 // use two slots so we can do a crossfade
#define CSD_THEME_ANIMATION_PLAYGROUND 2
#define CSD_THEME_ANIMATION_CRYSTALS   3

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
#define CSD_THEME_SAMPLE_HIGH_SCORE   11
#define CSD_THEME_SAMPLE_LOSE         12

typedef struct
{
	
	float x;
	float y;
	
} CSD_STAGE_OBJECT;

typedef struct
{
	
	CSD_STAGE_OBJECT playground;
	CSD_STAGE_OBJECT preview;
	CSD_STAGE_OBJECT score;
	CSD_STAGE_OBJECT level;
	CSD_STAGE_OBJECT message;
	CSD_STAGE_OBJECT block;
	CSD_STAGE_OBJECT board;
	
} CSD_STAGE_LAYOUT;

typedef struct
{
	
	/* data */
	T3F_ANIMATION * animation[CSD_THEME_ANIMATIONS];
	T3F_ANIMATION * crystal_animation[CSD_BLOCK_MAX_TYPES];
	T3F_ANIMATION * fcrystal_animation[CSD_BLOCK_MAX_TYPES];
	ALLEGRO_FONT * font;
	ALLEGRO_SAMPLE * sample[CSD_THEME_SAMPLES];
	T3F_ATLAS * atlas;
	int bg_slot;
	
	/* layout */
	CSD_STAGE_LAYOUT layout[CSD_MAX_PLAYERS];
	CSD_STAGE_OBJECT time;
	CSD_STAGE_OBJECT best_time;
	CSD_STAGE_OBJECT high_score;
	CSD_STAGE_OBJECT message;
	
	/* board settings */
	int board_width;
	int board_height;
	
	/* block settings */
	int block_width;
	int block_height;
	bool block_scale;
	int stack_height;
	int block_types;
	int flash_type;
	int fall_type;
	
	/* message settings */
	int message_width;
	int message_height;
	int message_scroll_type;
	
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
bool csd_load_stage(CSD_THEME * tp, CSD_STAGE * sp, int stage);
void csd_free_stage(CSD_STAGE * sp);

#endif
