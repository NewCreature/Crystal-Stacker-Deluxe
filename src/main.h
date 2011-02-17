#ifndef CSD_MAIN_H
#define CSD_MAIN_H

#include "t3net/t3net.h"

#define CSD_STATE_INTRO       0
#define CSD_STATE_TITLE       1
#define CSD_STATE_PLAYING     2
#define CSD_STATE_LEADERBOARD 3

#define CSD_MAX_BITMAPS   32

#define CSD_MAX_OPTIONS    64
#define CSD_OPTION_UPLOAD   0

extern int csd_option[CSD_MAX_OPTIONS];
extern int csd_high_score;
extern T3NET_LEADERBOARD * csd_leaderboard;
extern int csd_leaderboard_position;
extern char csd_upload_name[256];
extern int csd_tick;

void csd_exit(void);
void csd_set_state(int state);

#endif
