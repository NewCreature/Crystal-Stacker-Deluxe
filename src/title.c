#include "t3f/t3f.h"
#include "t3f/gui.h"
#include "main.h"
#include "title.h"
#include "game.h"

T3F_GUI * csd_menu[CSD_MAX_MENUS] = {NULL};
int csd_current_menu = CSD_MENU_MAIN;
ALLEGRO_FONT * csd_menu_font[CSD_MAX_MENU_FONTS] = {NULL};

int csd_menu_proc_main_play(int i, void * p)
{
	csd_game_setup();
	return 1;
}

int csd_menu_proc_main_options(int i, void * p)
{
	return 1;
}

int csd_menu_proc_main_leaderboards(int i, void * p)
{
	al_stop_timer(t3f_timer);
	if(csd_leaderboard)
	{
		t3net_destroy_leaderboard(csd_leaderboard);
	}
	csd_leaderboard = t3net_get_leaderboard("http://www.t3-i.com/leaderboards/query.php", "csd", "alpha", "classic", "0", 10, 0);
	if(csd_leaderboard)
	{
		csd_leaderboard_position = -1;
		csd_current_menu = CSD_MENU_LEADERBOARD;
		csd_set_state(CSD_STATE_LEADERBOARD);
	}
	al_start_timer(t3f_timer);
	return 1;
}

int csd_menu_proc_main_quit(int i, void * p)
{
	csd_exit();
	return 1;
}

int csd_menu_proc_leaderboard_exit(int i, void * p)
{
	csd_current_menu = CSD_MENU_MAIN;
	csd_set_state(CSD_STATE_TITLE);
	return 1;
}

int csd_menu_proc_leaderboard_replay(int i, void * p)
{
	csd_game_setup();
	return 1;
}

bool csd_title_setup(void)
{
	
	t3f_set_gui_driver(NULL); // use default driver for now
	
	/* load fonts */
	csd_menu_font[CSD_MENU_FONT_NORMAL] = al_load_font("fonts/main.ttf", 24, 0);
	if(!csd_menu_font[CSD_MENU_FONT_NORMAL])
	{
		return false;
	}
	
	/* main menu */
	csd_menu[CSD_MENU_MAIN] = t3f_create_gui(0, 0);
	if(!csd_menu[CSD_MENU_MAIN])
	{
		return false;
	}
	t3f_add_gui_text_element(csd_menu[CSD_MENU_MAIN], csd_menu_proc_main_play, "Play", csd_menu_font[CSD_MENU_FONT_NORMAL], 320, 0, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(csd_menu[CSD_MENU_MAIN], csd_menu_proc_main_options, "Options", csd_menu_font[CSD_MENU_FONT_NORMAL], 320, 24, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(csd_menu[CSD_MENU_MAIN], csd_menu_proc_main_leaderboards, "Leaderboards", csd_menu_font[CSD_MENU_FONT_NORMAL], 320, 48, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(csd_menu[CSD_MENU_MAIN], csd_menu_proc_main_quit, "Quit", csd_menu_font[CSD_MENU_FONT_NORMAL], 320, 72, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(csd_menu[CSD_MENU_MAIN], 0, 480);
	
	/* leaderboard menu */
	csd_menu[CSD_MENU_LEADERBOARD] = t3f_create_gui(0, 0);
	if(!csd_menu[CSD_MENU_LEADERBOARD])
	{
		return false;
	}
	t3f_add_gui_text_element(csd_menu[CSD_MENU_LEADERBOARD], csd_menu_proc_leaderboard_exit, "< Back", csd_menu_font[CSD_MENU_FONT_NORMAL], 0, 480 - 24, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_SHADOW);
	
	/* leaderboard menu (game over version) */
	csd_menu[CSD_MENU_LEADERBOARD_GO] = t3f_create_gui(0, 0);
	if(!csd_menu[CSD_MENU_LEADERBOARD_GO])
	{
		return false;
	}
	t3f_add_gui_text_element(csd_menu[CSD_MENU_LEADERBOARD_GO], csd_menu_proc_leaderboard_exit, "< Back to Menu", csd_menu_font[CSD_MENU_FONT_NORMAL], 0, 480 - 24, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(csd_menu[CSD_MENU_LEADERBOARD_GO], csd_menu_proc_leaderboard_replay, "Play Again >", csd_menu_font[CSD_MENU_FONT_NORMAL], 640 - al_get_text_width(csd_menu_font[CSD_MENU_FONT_NORMAL], "Play Again >"), 480 - 24, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_SHADOW);
	
	return true;
}

void csd_title_logic(void)
{
	t3f_process_gui(csd_menu[csd_current_menu]);
}

void csd_title_render(void)
{
	al_clear_to_color(al_map_rgba_f(0.1, 0.1, 0.1, 1.0));
	t3f_render_gui(csd_menu[csd_current_menu]);
}
