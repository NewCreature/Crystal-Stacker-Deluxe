#include <stdio.h>
#include "t3f/gui.h"
#include "main.h"
#include "title.h"

unsigned long csd_encode_leaderboard_score(unsigned long score)
{
	return score * 4 + ('c' + 's' + 'd' + 'g' + 'i' + 't') * 16;
}

unsigned long csd_decode_leaderboard_score(unsigned long score)
{
	return (score - ('c' + 's' + 'd' + 'g' + 'i' + 't') * 16) / 4;
}

void csd_leaderboard_logic(void)
{
	t3f_process_gui(csd_menu[csd_current_menu]);
}

void csd_leaderboard_render(void)
{
	int i;
	ALLEGRO_COLOR color;
	
	csd_title_render();
	
	al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], al_map_rgba(0, 0, 0, 128), 320 + 2, 8 + 2, ALLEGRO_ALIGN_CENTRE, "Global Leaderboard");
	al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], al_map_rgba(255, 255, 255, 255), 320, 8, ALLEGRO_ALIGN_CENTRE, "Global Leaderboard");
	
	for(i = 0; i < 10; i++)
	{
		if(i == csd_leaderboard_position && (csd_tick / 6) % 2 == 0)
		{
			color = al_map_rgba(255, 244, 141, 255);
		}
		else
		{
			color = al_map_rgba(255, 255, 255, 255);
		}
		if(strlen(csd_leaderboard->entry[i]->name) > 0)
		{
			al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], al_map_rgba(0, 0, 0, 128), 320 + 2 - 240, i * 32 + 2 + 64, ALLEGRO_ALIGN_LEFT, csd_leaderboard->entry[i]->name);
			al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], color, 320 - 240, i * 32 + 64, ALLEGRO_ALIGN_LEFT, csd_leaderboard->entry[i]->name);
			al_draw_textf(csd_menu_font[CSD_MENU_FONT_NORMAL], al_map_rgba(0, 0, 0, 128), 320 + 2 + 240, i * 32 + 2 + 64, ALLEGRO_ALIGN_RIGHT, "%lu", csd_decode_leaderboard_score(csd_leaderboard->entry[i]->score));
			al_draw_textf(csd_menu_font[CSD_MENU_FONT_NORMAL], color, 320 + 240, i * 32 + 64, ALLEGRO_ALIGN_RIGHT, "%lu", csd_decode_leaderboard_score(csd_leaderboard->entry[i]->score));
		}
		else
		{
			al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], al_map_rgba(0, 0, 0, 128), 320 + 2, i * 32 + 2 + 64, ALLEGRO_ALIGN_CENTRE, "...");
			al_draw_text(csd_menu_font[CSD_MENU_FONT_NORMAL], color, 320, i * 32 + 64, ALLEGRO_ALIGN_CENTRE, "...");
		}
	}
}
