#ifndef CSD_TITLE_H
#define CSD_TITLE_H

#include <allegro5/allegro_font.h>
#include "t3f/gui.h"

#define CSD_MAX_MENUS          32

#define CSD_MENU_MAIN           0
#define CSD_MENU_PLAY           1
#define CSD_MENU_LEADERBOARD    2
#define CSD_MENU_LEADERBOARD_GO 3
#define CSD_MENU_GAME_OVER      4

#define CSD_MAX_MENU_FONTS  32

#define CSD_MENU_FONT_NORMAL 0

extern T3F_GUI * csd_menu[CSD_MAX_MENUS];
extern int csd_current_menu;
extern ALLEGRO_FONT * csd_menu_font[CSD_MAX_MENU_FONTS];

bool csd_title_setup(void);
void csd_title_logic(void);
void csd_title_render(void);

#endif
