#include "t3f/t3f.h"
#include "t3f/music.h"
#include "main.h"
#include "title.h"
#include "game.h"
#include "leaderboard.h"
#include "core.h"

int level_chart[20] = {670, 600, 540, 480, 420, 370, 320, 280, 240, 200, 170, 140, 120, 100, 80, 70, 60, 55, 50, 45};
float ratio;
int down_done = 0;
bool high_broken = false;
int fill_height;

CSD_THEME * csd_theme = NULL;
CSD_GAME csd_game;

bool csd_play_sample(ALLEGRO_SAMPLE * sp)
{
	if(sp)
	{
		return al_play_sample(sp, 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	return false;
}

bool csd_game_setup(void)
{
	int i, j;
	
	al_stop_timer(t3f_timer);
	memset(&csd_game, 0, sizeof(CSD_GAME));
//	csd_theme = csd_load_theme("themes/default/theme.ini");
	csd_theme = csd_load_theme("themes/legacy/fb.cth");
	if(!csd_theme)
	{
		printf("Error loading theme!\n");
		return false;
	}
	csd_game.theme = csd_theme;
	
	/* load stages prior to starting stage */
	for(i = 0; i < csd_option[CSD_OPTION_START_LEVEL] - 1; i++)
	{
		csd_load_stage(csd_game.theme, &csd_game.stage, i);
	}
	csd_game_init_level(csd_option[CSD_OPTION_START_LEVEL], 0);
	for(i = 0; i < csd_game.stage.board_height; i++)
	{
		for(j = 0; j < csd_game.stage.board_width; j++)
		{
			csd_game.player[0].board.data[i][j] = 0;
			csd_game.player[0].board.flag[i][j] = 0;
		}
	}
	csd_game.players = 1;
	for(i = 0; i < csd_game.players; i++)
	{
		csd_game.player[i].block.x = (csd_game.stage.board_width / 2) * csd_game.stage.block_width;
		csd_game.player[i].block.y = 0.0;
		csd_player_block_generate(&csd_game.player[i].block);
		csd_player_block_generate(&csd_game.player[i].block_preview);
		csd_game.player[i].id = i;
		csd_game.player[i].board.id = i;
		csd_game.player[i].block.id = i;
		csd_game_add_player_message(i, CSD_MESSAGE_SCROLL, "Get your match on!");
	}
	memset(csd_controller_repeat, 0, sizeof(int) * CSD_MAX_PLAYERS * 5);
	high_broken = false;
	csd_game.state = CSD_GAME_STATE_PLAY;
	csd_set_state(CSD_STATE_PLAYING);
	al_start_timer(t3f_timer);
	return true;
}

void csd_game_exit(void)
{
	char buffer[256] = {0};
	int i;
	
	/* handle high scores */
	sprintf(buffer, "%d", csd_high_score);
	al_set_config_value(t3f_config, "Game Data", "High Score", buffer);
	if(csd_option[CSD_OPTION_UPLOAD])
	{
		al_stop_timer(t3f_timer);
		t3net_upload_score("http://www.t3-i.com/leaderboards/poll.php", "csd", "alpha", "classic", "0", csd_upload_name, csd_encode_leaderboard_score(csd_game.player[0].score));
		if(csd_leaderboard)
		{
			t3net_destroy_leaderboard(csd_leaderboard);
		}
		csd_leaderboard = t3net_get_leaderboard("http://www.t3-i.com/leaderboards/query.php", "csd", "alpha", "classic", "0", 10, 0);
		if(csd_leaderboard)
		{
			csd_leaderboard_position = -1;
			for(i = 0; i < csd_leaderboard->entries; i++)
			{
				if(csd_game.player[0].score == csd_decode_leaderboard_score(csd_leaderboard->entry[i]->score) && !strcmp(csd_upload_name, csd_leaderboard->entry[i]->name))
				{
					csd_leaderboard_position = i;
					break;
				}
			}
			csd_current_menu = CSD_MENU_LEADERBOARD_GO;
			csd_set_state(CSD_STATE_LEADERBOARD);
//			lingo_state = LINGO_STATE_LEADERBOARD;
//			lingo_current_menu = LINGO_MENU_LEADERBOARD;
		}
		else
		{
			csd_current_menu = CSD_MENU_MAIN;
			csd_set_state(CSD_STATE_TITLE);
//			lingo_game_state = LINGO_GAME_STATE_OVER;
//			lingo_current_game_menu = LINGO_GAME_MENU_OVER;
		}
		al_start_timer(t3f_timer);
	}
	else
	{
		csd_current_menu = CSD_MENU_MAIN;
		csd_set_state(CSD_STATE_TITLE);
	}
	
	csd_free_stage(&csd_game.stage);
	t3f_stop_music();
}

void csd_game_init_level(int level, int player)
{
	csd_game.player[player].level = level;
	csd_load_stage(csd_game.theme, &csd_game.stage, csd_game.player[player].level);
	
	/* increase the speed of the stack */
	ratio = (float)level_chart[csd_game.player[player].level] / 1000.0;
	csd_game.player[player].block.vy = (float)csd_game.stage.block_height / (ratio * 60.0);
	csd_game.player[player].board.width = csd_game.stage.board_width;
	csd_game.player[player].board.height = csd_game.stage.board_height + csd_game.stage.stack_height;
}

bool csd_game_add_player_message(int player, int type, const char * message)
{
	return csd_queue_message(&csd_game.player[player].messages, type, message, csd_game.stage.message_width);
}

/* run game logic for a player */
void csd_game_player_logic(int player)
{
	int i, temp;
	bool move;
	
	if(!csd_game.player[player].lost)
	{
		t3f_read_controller(csd_controller[player]);
		t3f_update_controller(csd_controller[player]);
		switch(csd_game.player[player].board.state)
		{
			case CSD_PLAYER_BOARD_STATE_NORMAL:
			{
				if(csd_controller[player]->state[CSD_CONTROLLER_LEFT].held)
				{
					move = false;
					csd_controller_repeat[player][CSD_CONTROLLER_LEFT]++;
					if(csd_controller_repeat[player][CSD_CONTROLLER_LEFT] == 1)
					{
						move = true;
					}
					else if(csd_controller_repeat[player][CSD_CONTROLLER_LEFT] >= CSD_REPEAT_DELAY && (csd_controller_repeat[player][CSD_CONTROLLER_LEFT] - CSD_REPEAT_DELAY) % CSD_REPEAT_SPEED == 0)
					{
						move = true;
					}
					if(move)
					{
						if(csd_game.player[player].block.bx > 0 && csd_game.player[player].board.data[csd_game.player[player].block.by + 3][csd_game.player[player].block.bx - 1] == 0)
						{
							csd_game.player[player].block.bx--;
							csd_game.player[player].block.x = csd_game.player[player].block.bx * csd_game.stage.block_width;
							csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MOVE]);
						}
						else
						{
							csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MOVE_FAIL]);
						}
					}
				}
				else
				{
					csd_controller_repeat[player][CSD_CONTROLLER_LEFT] = 0;
				}
				if(csd_controller[player]->state[CSD_CONTROLLER_RIGHT].held)
				{
					move = false;
					csd_controller_repeat[player][CSD_CONTROLLER_RIGHT]++;
					if(csd_controller_repeat[player][CSD_CONTROLLER_RIGHT] == 1)
					{
						move = true;
					}
					else if(csd_controller_repeat[player][CSD_CONTROLLER_RIGHT] >= CSD_REPEAT_DELAY && (csd_controller_repeat[player][CSD_CONTROLLER_RIGHT] - CSD_REPEAT_DELAY) % CSD_REPEAT_SPEED == 0)
					{
						move = true;
					}
					if(move)
					{
						if(csd_game.player[player].block.bx < csd_game.stage.board_width - 1 && csd_game.player[player].board.data[csd_game.player[player].block.by + 3][csd_game.player[player].block.bx + 1] == 0)
						{
							csd_game.player[player].block.bx++;
							csd_game.player[player].block.x = csd_game.player[player].block.bx * csd_game.stage.block_width;
							csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MOVE]);
						}
						else
						{
							csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MOVE_FAIL]);
						}
					}
				}
				else
				{
					csd_controller_repeat[player][CSD_CONTROLLER_RIGHT] = 0;
				}
				if(csd_controller[player]->state[CSD_CONTROLLER_DROP].held && !down_done)
				{
					if(csd_game.stage.fall_type == 1)
					{
						csd_game.player[player].block.y += (float)csd_game.stage.block_height - fmod(csd_game.player[player].block.y, csd_game.stage.block_height);
					}
					else
					{
						csd_game.player[player].block.y += (float)csd_game.stage.block_height;
					}
					csd_game.player[player].block.by = csd_game.player[player].block.y / csd_game.stage.block_height;
				}
				else
				{
					csd_game.player[player].block.y += csd_game.player[player].block.vy;
					if(!csd_controller[player]->state[CSD_CONTROLLER_DROP].held)
					{
						down_done = 0;
					}
				}
				if(csd_controller[player]->state[CSD_CONTROLLER_RUP].pressed)
				{
					/* remember the top crystal */
					temp = csd_game.player[player].block.data[0];
			
					/* move the others up */
					for(i = 0; i < csd_game.stage.stack_height; i++)
					{
						csd_game.player[player].block.data[i] = csd_game.player[player].block.data[i + 1];
					}
			
					/* place remembered crystal at the bottom */
					csd_game.player[player].block.data[csd_game.stage.stack_height - 1] = temp;
					csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_ROTATE_UP]);
				}
				if(csd_controller[player]->state[CSD_CONTROLLER_RDOWN].pressed)
				{
					/* remember the top crystal */
					temp = csd_game.player[player].block.data[2];
			
					/* move the others up */
					for(i = csd_game.stage.stack_height - 1; i >= 0; i--)
					{
						csd_game.player[player].block.data[i] = csd_game.player[player].block.data[i - 1];
					}
			
					/* place remembered crystal at the bottom */
					csd_game.player[player].block.data[0] = temp;
					csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_ROTATE_DOWN]);
				}
				csd_game.player[player].block.bx = csd_game.player[player].block.x / csd_game.stage.block_width;
				csd_game.player[player].block.by = csd_game.player[player].block.y / csd_game.stage.block_height;
				
				/* adjust for negative number */
				if(csd_game.player[player].block.y < 0)
				{
					csd_game.player[player].block.by--;
				}
				if(csd_game.player[player].block.by >= csd_game.stage.board_height || csd_game.player[player].board.data[csd_game.player[player].block.by + csd_game.stage.stack_height][csd_game.player[player].block.bx] != 0)
				{
					down_done = 1;
					if(csd_game.player[player].block.by < csd_game.stage.stack_height)
					{
						csd_game.player[player].lost = true;
						fill_height = 0;
						csd_game.state = CSD_GAME_STATE_OVER;
						csd_game_add_player_message(player, CSD_MESSAGE_SCROLL, "You lost!");
						return;
					}
					if(csd_game.player[player].block.data[0] != CSD_BLOCK_TYPE_BOMB)
					{
						for(i = 0; i < csd_game.stage.stack_height; i++)
						{
							csd_game.player[player].board.data[csd_game.player[player].block.by + i][csd_game.player[player].block.bx] = csd_game.player[player].block.data[i];
						}
					}
					else
					{
						/* bomb hit bottom so no blocks are bombed */
						if(csd_game.player[player].block.by >= csd_game.stage.board_height)
						{
							for(i = 0; i < csd_game.stage.stack_height; i++)
							{
								csd_game.player[player].board.data[csd_game.player[player].block.by + i][csd_game.player[player].block.bx] = csd_game.player[player].block.data[i];
							}
						}
						else
						{
							for(i = 0; i < csd_game.stage.stack_height; i++)
							{
								csd_game.player[player].board.data[csd_game.player[player].block.by + i][csd_game.player[player].block.bx] = csd_game.player[player].board.data[csd_game.player[player].block.by + 3][csd_game.player[player].block.bx];
							}
							core_mark_runs_bomb(&csd_game.player[player].board, csd_game.player[player].block.bx, csd_game.player[player].block.by + 3);
						}
					}
					csd_game.player[player].block.x = (csd_game.stage.board_width / 2) * csd_game.stage.block_width;
					csd_game.player[player].block.y = 0.0;
					csd_player_block_copy(&csd_game.player[player].block, &csd_game.player[player].block_preview);
					csd_player_block_generate(&csd_game.player[player].block_preview);
					csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_LAND]);
					if(core_find_all_runs(&csd_game.player[player].board) > 0)
					{
						csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MATCHES]);
						csd_game.player[player].board.state = CSD_PLAYER_BOARD_STATE_RUNS;
						csd_game.player[player].board.state_counter = 0;
						csd_game.player[player].combo = 0;
						csd_game.player[player].removed = 0;
					}
				}
				break;
			}
			case CSD_PLAYER_BOARD_STATE_RUNS:
			{
				csd_game.player[player].board.state_counter++;
				if(csd_game.player[player].board.state_counter >= 18)
				{
					temp = core_delete_runs(&csd_game.player[player].board);
					csd_game.player[player].destroyed += temp;
					csd_game.player[player].removed += temp;
					csd_game.player[player].combo++;
					csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_DELETE]);
					if(csd_game.player[player].destroyed >= (csd_game.player[player].level + 1) * 45)
					{
						if(csd_game.player[player].level < 20)
						{
							csd_game_init_level(csd_game.player[player].level + 1, 0);
							csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_LEVEL]);
						}
					}
					if(core_find_all_runs(&csd_game.player[player].board) > 0)
					{
						csd_play_sample(csd_game.stage.sample[CSD_THEME_SAMPLE_MATCHES]);
						csd_game.player[player].board.state_counter = 0;
					}
					else
					{
						csd_game.player[player].score += csd_game.player[player].combo * (((csd_game.player[player].removed * 5) * ((csd_game.player[player].level * 100) / 4 + 100)) / 100);
						if(csd_game.player[player].score > csd_high_score)
						{
							if(!high_broken)
							{
								csd_game_add_player_message(player, CSD_MESSAGE_SCROLL, "You beat the high score!");
								high_broken = true;
							}
							csd_high_score = csd_game.player[player].score;
						}
						csd_game.player[player].board.state = CSD_PLAYER_BOARD_STATE_NORMAL;
					}
				}
				break;
			}
		}
	}
	csd_process_message_queue(&csd_game.player[player].messages, csd_game.stage.font);
}

void csd_game_projectile_logic(void)
{
	int i;
	
	for(i = 0; i < 64; i++)
	{
		sprite_3d_logic(&csd_game.sprite[i]);
		if(csd_game.sprite[i].y > 480.0)
		{
			csd_game.sprite[i].active = false;
		}
	}
}

void csd_game_logic(void)
{
	int i, j;
	
	switch(csd_game.state)
	{
		case CSD_GAME_STATE_PLAY:
		{
			if(t3f_key[ALLEGRO_KEY_P])
			{
				csd_game.state = CSD_GAME_STATE_PAUSE;
				t3f_key[ALLEGRO_KEY_P] = 0;
			}
			for(i = 0; i < csd_game.players; i++)
			{
				csd_game_player_logic(i);
			}
			csd_game_projectile_logic();
			break;
		}
		case CSD_GAME_STATE_PAUSE:
		{
			if(t3f_key[ALLEGRO_KEY_P])
			{
				csd_game.state = CSD_GAME_STATE_PLAY;
				t3f_key[ALLEGRO_KEY_P] = 0;
			}
			csd_game_projectile_logic();
			break;
		}
		case CSD_GAME_STATE_OVER:
		{
			for(i = 0; i < csd_game.players; i++)
			{
				csd_game_player_logic(i);
			}
			if(fill_height <= csd_game.stage.board_height)
			{
				if(csd_game.tick % 3 == 0)
				{
					for(i = 0; i < csd_game.players; i++)
					{
						if(csd_game.player[i].lost)
						{
							for(j = 0; j < csd_game.stage.board_width; j++)
							{
								csd_game.player[i].board.data[csd_game.stage.board_height + 3 - 1 - fill_height][j] = CSD_BLOCK_TYPE_SOLID;
							}
						}
					}
					fill_height++;
				}
			}
			else if(csd_game.player[0].messages.messages == 0)
			{
				
				/* go to leaderboards if upload is enabled */
				if(csd_option[CSD_OPTION_UPLOAD])
				{
					csd_game_exit();
				}
				
				/* otherwise show menu */
				else
				{
					csd_current_menu = CSD_MENU_GAME_OVER;
					csd_game.state = CSD_GAME_STATE_OVER_MENU;
				}
			}
			break;
		}
		case CSD_GAME_STATE_OVER_MENU:
		{
			t3f_process_gui(csd_menu[csd_current_menu]);
			break;
		}
	}
	csd_game.tick++;
}

void csd_game_player_render(int player)
{
	int i, j;
	float sy;
	
	/* draw board */
	int cx, cy, cw, ch;
	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
	t3f_set_clipping_rectangle(csd_game.stage.layout[player].playground.x, csd_game.stage.layout[player].playground.y, csd_game.stage.layout[player].playground.x + csd_game.stage.board_width * csd_game.stage.block_width, csd_game.stage.layout[player].playground.y + csd_game.stage.board_height * csd_game.stage.block_height);
	al_hold_bitmap_drawing(true);
	for(i = 0; i < csd_game.stage.board_height; i++)
	{
		for(j = 0; j < csd_game.stage.board_width; j++)
		{
			if(csd_game.player[player].board.data[i + csd_game.stage.stack_height][j] > 0 && csd_game.player[player].board.data[i + csd_game.stage.stack_height][j] < CSD_BLOCK_MAX_TYPES)
			{
				if(csd_game.player[player].board.state == CSD_PLAYER_BOARD_STATE_NORMAL || (csd_game.player[player].board.state_counter / 3) % 2 == 0 || csd_game.player[player].board.flag[i + csd_game.stage.stack_height][j] == 0)
				{
					t3f_draw_animation(csd_game.stage.crystal_animation[(int)csd_game.player[player].board.data[i + csd_game.stage.stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, j * csd_game.stage.block_width + csd_game.stage.layout[player].playground.x, i * csd_game.stage.block_height + csd_game.stage.layout[player].playground.y, 0.0, 0);
				}
				else
				{
					if(csd_game.stage.flash_type == CSD_THEME_FLASH_OVERLAY)
					{
						t3f_draw_animation(csd_game.stage.crystal_animation[(int)csd_game.player[player].board.data[i + csd_game.stage.stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, j * csd_game.stage.block_width + csd_game.stage.layout[player].playground.x, i * csd_game.stage.block_height + csd_game.stage.layout[player].playground.y, 0.0, 0);
						t3f_draw_animation(csd_game.stage.fcrystal_animation[0], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, j * csd_game.stage.block_width + csd_game.stage.layout[player].playground.x, i * csd_game.stage.block_height + csd_game.stage.layout[player].playground.y, 0.0, 0);
					}
					else
					{
						t3f_draw_animation(csd_game.stage.fcrystal_animation[(int)csd_game.player[player].board.data[i + csd_game.stage.stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, j * csd_game.stage.block_width + csd_game.stage.layout[player].playground.x, i * csd_game.stage.block_height + csd_game.stage.layout[player].playground.y, 0.0, 0);
					}
				}
			}
		}
	}
	if(!csd_game.player[player].lost)
	{
		for(i = 0; i < csd_game.stage.stack_height; i++)
		{
			if(csd_game.stage.fall_type == 0)
			{
				sy = csd_game.player[player].block.y;
			}
			else
			{
				sy = ((int)(csd_game.player[player].block.y + (csd_game.stage.block_height - 1)) / csd_game.stage.block_height) * csd_game.stage.block_height;
			}
			t3f_draw_animation(csd_game.stage.crystal_animation[(int)csd_game.player[player].block.data[i]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, csd_game.stage.layout[player].playground.x + csd_game.player[player].block.x, (i - csd_game.stage.stack_height) * csd_game.stage.block_height + csd_game.stage.layout[player].playground.y + sy, 0.0, 0);
		}
	}
	al_hold_bitmap_drawing(false);
	t3f_set_clipping_rectangle(csd_game.stage.layout[player].message.x, csd_game.stage.layout[player].message.y, csd_game.stage.message_width, csd_game.stage.message_height);
	csd_render_message_queue(&csd_game.player[player].messages, csd_game.stage.font, csd_game.stage.layout[player].message.x, csd_game.stage.layout[player].message.y, csd_game.stage.message_scroll_type == 0);
	al_set_clipping_rectangle(cx, cy, cw, ch);
	
	/* draw preview block */
	al_hold_bitmap_drawing(true);
	if(!csd_game.player[player].lost)
	{
		for(i = 0; i < csd_game.stage.stack_height; i++)
		{
			t3f_draw_animation(csd_game.stage.crystal_animation[(int)csd_game.player[player].block_preview.data[i]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), csd_game.tick, csd_game.stage.layout[player].preview.x, i * csd_game.stage.block_height + csd_game.stage.layout[player].preview.y, 0.0, 0);
		}
	}
	
	/* draw game info */
	al_draw_textf(csd_game.stage.font, t3f_color_white, csd_game.stage.layout[player].score.x, csd_game.stage.layout[player].score.y, 0, "%05d", csd_game.player[player].score);
	al_draw_textf(csd_game.stage.font, t3f_color_white, csd_game.stage.layout[player].block.x, csd_game.stage.layout[player].block.y, 0, "%04d", csd_game.player[player].destroyed);
	al_draw_textf(csd_game.stage.font, t3f_color_white, csd_game.stage.layout[player].level.x, csd_game.stage.layout[player].level.y, 0, "%02d", csd_game.player[player].level + 1);
	al_hold_bitmap_drawing(false);
}

void csd_game_render(void)
{
	int i;
	
	if(csd_game.stage.animation[CSD_THEME_ANIMATION_BACKGROUND])
	{
		t3f_draw_animation(csd_game.stage.animation[CSD_THEME_ANIMATION_BACKGROUND], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0.0, 0.0, 0.0, 0);
	}
	else
	{
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	}
	if(csd_game.stage.animation[CSD_THEME_ANIMATION_PLAYGROUND])
	{
		t3f_draw_animation(csd_game.stage.animation[CSD_THEME_ANIMATION_PLAYGROUND], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, csd_game.stage.layout[0].playground.x, csd_game.stage.layout[0].playground.y, 0.0, 0);
	}

	for(i = 0; i < csd_game.players; i++)
	{
		csd_game_player_render(i);
	}
	al_draw_textf(csd_game.stage.font, t3f_color_white, csd_game.stage.high_score.x, csd_game.stage.high_score.y, 0, "%05d", csd_high_score);
	
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 64; i++)
	{
		sprite_3d_draw(&csd_game.sprite[i]);
	}
	al_hold_bitmap_drawing(false);
	if(csd_game.state == CSD_GAME_STATE_OVER_MENU)
	{
		al_draw_filled_rectangle(0, 0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
		t3f_render_gui(csd_menu[csd_current_menu]);
	}
}
