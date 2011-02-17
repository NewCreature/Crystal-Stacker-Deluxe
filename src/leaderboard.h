#ifndef CSD_LEADERBOARD_H
#define CSD_LEADERBOARD_H

unsigned long csd_encode_leaderboard_score(unsigned long score);
unsigned long csd_decode_leaderboard_score(unsigned long score);

void csd_leaderboard_logic(void);
void csd_leaderboard_render(void);

#endif
