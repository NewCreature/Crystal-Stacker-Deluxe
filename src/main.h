#ifndef CSD_MAIN_H
#define CSD_MAIN_H

#define CSD_STATE_INTRO    0
#define CSD_STATE_TITLE    1
#define CSD_STATE_PLAYING  2

#define CSD_MAX_BITMAPS   32

extern int csd_high_score;

void csd_exit(void);
void csd_set_state(int state);

#endif
