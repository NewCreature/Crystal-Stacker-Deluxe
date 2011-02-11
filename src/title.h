#ifndef CSD_TITLE_H
#define CSD_TITLE_H

#define CSD_MAX_MENUS       32

#define CSD_MENU_MAIN        0
#define CSD_MENU_PLAY        1

#define CSD_MAX_MENU_FONTS  32

#define CSD_MENU_FONT_NORMAL 0

bool csd_title_setup(void);
void csd_title_logic(void);
void csd_title_render(void);

#endif
