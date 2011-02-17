#ifndef CSD_MAIN_H
#define CSD_MAIN_H

#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "t3net/t3net.h"
#include "game.h"

#define CSD_STATE_INTRO        0
#define CSD_STATE_TITLE        1
#define CSD_STATE_PLAYING      2
#define CSD_STATE_LEADERBOARD  3

#define CSD_MAX_BITMAPS       32
#define CSD_BITMAP_TITLE_BG    0
#define CSD_BITMAP_TITLE_LOGO  1

#define CSD_MAX_OPTIONS       64
#define CSD_OPTION_UPLOAD      0
#define CSD_OPTION_START_LEVEL 1

#define CSD_CONTROLLER_LEFT    0
#define CSD_CONTROLLER_RIGHT   1
#define CSD_CONTROLLER_DROP    2
#define CSD_CONTROLLER_RUP     3
#define CSD_CONTROLLER_RDOWN   4

extern ALLEGRO_BITMAP * csd_bitmap[CSD_MAX_BITMAPS];
extern int csd_option[CSD_MAX_OPTIONS];
extern int csd_high_score;
extern T3NET_LEADERBOARD * csd_leaderboard;
extern int csd_leaderboard_position;
extern char csd_upload_name[256];
extern int csd_tick;
extern T3F_CONTROLLER * csd_controller[CSD_MAX_PLAYERS];
extern int csd_controller_repeat[CSD_MAX_PLAYERS][5];

void csd_exit(void);
void csd_set_state(int state);

#endif
