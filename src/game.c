#include <allegro5/allegro5.h>
#include "t3f/t3f.h"
#include "game.h"
#include "core.h"

extern int done;
extern int frame;
extern int level_chart[20];
extern float ratio;
int down_done = 0;

void csd_game_init_level(CSD_GAME * gp, int level, int player)
{
	gp->player[player].level = level;
	if(csd_should_load_stage(gp->theme, gp->player[player].level))
	{
		if(gp->stage)
		{
			csd_destroy_stage(gp->stage);
		}
		gp->stage = csd_load_stage(gp->theme, gp->player[player].level);
		if(!gp->stage)
		{
			printf("Error loading stage!\n");
		}
	}
	
	/* increase the speed of the stack */
	ratio = (float)level_chart[gp->player[player].level] / 1000.0;
	gp->player[player].block.vy = (float)gp->stage->crystal_animation[0]->frame[0]->height / (ratio * 60.0);
	gp->player[player].board.width = gp->stage->board_width;
	gp->player[player].board.height = gp->stage->board_height + gp->stage->stack_height;
}

/* run game logic for a player */
void csd_game_player_logic(CSD_GAME * gp, int player)
{
	int i, temp;
	
	switch(gp->player[player].board.state)
	{
		case CSD_PLAYER_BOARD_STATE_NORMAL:
		{
			if(t3f_key[ALLEGRO_KEY_LEFT])
			{
				if(gp->player[player].block.bx > 0 && gp->player[player].board.data[gp->player[player].block.by + 3][gp->player[player].block.bx - 1] == 0)
				{
					gp->player[player].block.bx--;
					gp->player[player].block.x = gp->player[player].block.bx * gp->stage->crystal_animation[0]->frame[0]->width;
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MOVE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				else
				{
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MOVE_FAIL], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				t3f_key[ALLEGRO_KEY_LEFT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_RIGHT])
			{
				if(gp->player[player].block.bx < gp->stage->board_width - 1 && gp->player[player].board.data[gp->player[player].block.by + 3][gp->player[player].block.bx + 1] == 0)
				{
					gp->player[player].block.bx++;
					gp->player[player].block.x = gp->player[player].block.bx * gp->stage->crystal_animation[0]->frame[0]->width;
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MOVE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				else
				{
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MOVE_FAIL], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				t3f_key[ALLEGRO_KEY_RIGHT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN] && !down_done)
			{
				gp->player[player].block.y += gp->stage->crystal_animation[0]->frame[0]->height;
				gp->player[player].block.by = gp->player[player].block.y / gp->stage->crystal_animation[0]->frame[0]->height;
			}
			else
			{
				gp->player[player].block.y += gp->player[player].block.vy;
				if(!t3f_key[ALLEGRO_KEY_DOWN])
				{
					down_done = 0;
				}
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				/* remember the top crystal */
				temp = gp->player[player].block.data[0];
		
				/* move the others up */
				for(i = 0; i < gp->stage->stack_height; i++)
				{
	    			gp->player[player].block.data[i] = gp->player[player].block.data[i + 1];
				}
		
				/* place remembered crystal at the bottom */
				gp->player[player].block.data[gp->stage->stack_height - 1] = temp;
				al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_ROTATE_UP], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			gp->player[player].block.bx = gp->player[player].block.x / gp->stage->crystal_animation[0]->frame[0]->width;
			gp->player[player].block.by = gp->player[player].block.y / gp->stage->crystal_animation[0]->frame[0]->height;
			
			/* adjust for negative number */
			if(gp->player[player].block.y < 0)
			{
				gp->player[player].block.by--;
			}
			if(gp->player[player].block.by >= gp->stage->board_height || gp->player[player].board.data[gp->player[player].block.by + gp->stage->stack_height][gp->player[player].block.bx] != 0)
			{
				down_done = 1;
				if(gp->player[player].block.by < gp->stage->stack_height)
				{
					done = 1;
					return;
				}
				if(gp->player[player].block.data[0] != CSD_BLOCK_TYPE_BOMB)
				{
					for(i = 0; i < gp->stage->stack_height; i++)
					{
						gp->player[player].board.data[gp->player[player].block.by + i][gp->player[player].block.bx] = gp->player[player].block.data[i];
					}
				}
				else
				{
					/* bomb hit bottom so no blocks are bombed */
					if(gp->player[player].block.by >= gp->stage->board_height)
					{
						for(i = 0; i < gp->stage->stack_height; i++)
						{
							gp->player[player].board.data[gp->player[player].block.by + i][gp->player[player].block.bx] = gp->player[player].block.data[i];
						}
					}
					else
					{
						for(i = 0; i < gp->stage->stack_height; i++)
						{
							gp->player[player].board.data[gp->player[player].block.by + i][gp->player[player].block.bx] = gp->player[player].board.data[gp->player[player].block.by + 3][gp->player[player].block.bx];
						}
						core_mark_runs_bomb(&gp->player[player].board, gp->player[player].block.bx, gp->player[player].block.by + 3);
					}
				}
				gp->player[player].block.x = (gp->stage->board_width / 2) * gp->stage->crystal_animation[0]->frame[0]->width;
				gp->player[player].block.y = 0.0;
				csd_player_block_copy(&gp->player[player].block, &gp->player[player].block_preview);
				csd_player_block_generate(&gp->player[player].block_preview);
				al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_LAND], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				if(core_find_all_runs(&gp->player[player].board) > 0)
				{
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MATCHES], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					gp->player[player].board.state = CSD_PLAYER_BOARD_STATE_RUNS;
					gp->player[player].board.state_counter = 0;
					gp->player[player].combo = 0;
					gp->player[player].removed = 0;
				}
			}
			break;
		}
		case CSD_PLAYER_BOARD_STATE_RUNS:
		{
			gp->player[player].board.state_counter++;
			if(gp->player[player].board.state_counter >= 18)
			{
				temp = core_delete_runs(&gp->player[player].board);
				gp->player[player].destroyed += temp;
				gp->player[player].removed += temp;
				gp->player[player].combo++;
				al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_DELETE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    		if(gp->player[player].destroyed >= (gp->player[player].level + 1) * 45)
				{
					if(gp->player[player].level < 20)
					{
	    				csd_game_init_level(gp, gp->player[player].level + 1, 0);
						al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_LEVEL], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
				if(core_find_all_runs(&gp->player[player].board) > 0)
				{
					al_play_sample(gp->stage->sample[CSD_THEME_SAMPLE_MATCHES], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					gp->player[player].board.state_counter = 0;
				}
				else
				{
		            gp->player[player].score += gp->player[player].combo * (((gp->player[player].removed * 5) * ((gp->player[player].level * 100) / 4 + 100)) / 100);
					gp->player[player].board.state = CSD_PLAYER_BOARD_STATE_NORMAL;
				}
			}
			break;
		}
	}
}

void csd_game_logic(CSD_GAME * gp)
{
	int i;
	
	switch(gp->state)
	{
		case CSD_GAME_STATE_PLAY:
		{
			if(t3f_key[ALLEGRO_KEY_P])
			{
				gp->state = CSD_GAME_STATE_PAUSE;
				t3f_key[ALLEGRO_KEY_P] = 0;
			}
			for(i = 0; i < gp->players; i++)
			{
				csd_game_player_logic(gp, i);
			}
			break;
		}
		case CSD_GAME_STATE_PAUSE:
		{
			if(t3f_key[ALLEGRO_KEY_P])
			{
				gp->state = CSD_GAME_STATE_PLAY;
				t3f_key[ALLEGRO_KEY_P] = 0;
			}
			break;
		}
	}
	
	for(i = 0; i < 64; i++)
	{
		sprite_3d_logic(&gp->sprite[i]);
		if(gp->sprite[i].y > 480.0)
		{
			gp->sprite[i].active = false;
		}
	}
}

void csd_game_player_render(CSD_GAME * gp, int player)
{
	int i, j;
	/* draw board */
/*	if(gp->theme->flag[CSD_THEME_FLAG_CLIP_BOARD_TOP])
	{
		cy = gp->theme->flag[CSD_THEME_FLAG_BOARD0_OY];
	}
	if(gp->theme->flag[CSD_THEME_FLAG_CLIP_BOARD_BOTTOM])
	{
		cb = gp->theme->flag[CSD_THEME_FLAG_BOARD0_OY] + gp->theme->flag[CSD_THEME_FLAG_BOARD_H] * gp->theme->flag[CSD_THEME_FLAG_BLOCK_H];
	}
	if(gp->theme->flag[CSD_THEME_FLAG_CLIP_BOARD_LEFT])
	{
		cx = gp->theme->flag[CSD_THEME_FLAG_BOARD0_OX];
	}
	if(gp->theme->flag[CSD_THEME_FLAG_CLIP_BOARD_RIGHT])
	{
		cr = gp->theme->flag[CSD_THEME_FLAG_BOARD0_OX] + gp->theme->flag[CSD_THEME_FLAG_BOARD_W] * gp->theme->flag[CSD_THEME_FLAG_BLOCK_W];
	} */
//	set_clip(gp->ibuffer[0], cx, cy, cr, cb);
	int cx, cy, cw, ch;
	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
//	al_set_clipping_rectangle((float)gp->stage->playground_x[player] * t3f_3d_state.scale_x, (float)gp->stage->playground_y[player] * t3f_3d_state.scale_y, (float)(gp->stage->board_width * gp->stage->crystal_animation[0]->frame[0]->width) * t3f_3d_state.scale_x, (float)((gp->stage->board_height) * gp->stage->crystal_animation[0]->frame[0]->height) * t3f_3d_state.scale_y);
	for(i = 0; i < gp->stage->board_height; i++)
	{
		for(j = 0; j < gp->stage->board_width; j++)
		{
			if(gp->player[player].board.data[i + gp->stage->stack_height][j] > 0 && gp->player[player].board.data[i + gp->stage->stack_height][j] < CSD_BLOCK_MAX_TYPES)
			{
				if(gp->player[player].board.state == CSD_PLAYER_BOARD_STATE_NORMAL || (gp->player[player].board.state_counter / 3) % 2 == 0 || gp->player[player].board.flag[i + gp->stage->stack_height][j] == 0)
				{
					t3f_draw_animation(gp->stage->crystal_animation[(int)gp->player[player].board.data[i + gp->stage->stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, j * gp->stage->crystal_animation[0]->frame[0]->width + gp->stage->playground_x[player], i * gp->stage->crystal_animation[0]->frame[0]->height + gp->stage->playground_y[player], 0.0, 0);
				}
				else
				{
					if(gp->stage->flash_type == CSD_THEME_FLASH_OVERLAY)
					{
						t3f_draw_animation(gp->stage->crystal_animation[(int)gp->player[player].board.data[i + gp->stage->stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, j * gp->stage->crystal_animation[0]->frame[0]->width + gp->stage->playground_x[player], i * gp->stage->crystal_animation[0]->frame[0]->height + gp->stage->playground_y[player], 0.0, 0);
						t3f_draw_animation(gp->stage->fcrystal_animation[0], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), frame, j * gp->stage->fcrystal_animation[0]->frame[0]->width + gp->stage->playground_x[player], i * gp->stage->fcrystal_animation[0]->frame[0]->height + gp->stage->playground_y[player], 0.0, 0);
					}
					else
					{
						t3f_draw_animation(gp->stage->crystal_animation[(int)gp->player[player].board.data[i + gp->stage->stack_height][j]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), frame, j * gp->stage->crystal_animation[0]->frame[0]->width + gp->stage->playground_x[player], i * gp->stage->crystal_animation[0]->frame[0]->height + gp->stage->playground_y[player], 0.0, 0);
					}
				}
			}
		}
	}
	for(i = 0; i < gp->stage->stack_height; i++)
	{
		t3f_draw_animation(gp->stage->crystal_animation[(int)gp->player[player].block.data[i]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, gp->stage->playground_x[player] + gp->player[player].block.x, (i - gp->stage->stack_height) * gp->stage->crystal_animation[0]->frame[0]->height + gp->stage->playground_y[player] + gp->player[player].block.y, 0.0, 0);
	}
	al_set_clipping_rectangle(cx, cy, cw, ch);
//	set_clip(gp->ibuffer[0], 0, 0, bp->w - 1, bp->h - 1);
	
	/* draw preview block */
	for(i = 0; i < gp->stage->stack_height; i++)
	{
		t3f_draw_animation(gp->stage->crystal_animation[(int)gp->player[player].block_preview.data[i]], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, gp->stage->preview_x[player], i * gp->stage->crystal_animation[0]->frame[0]->height + gp->stage->preview_y[player], 0.0, 0);
	}
	
	/* draw game info */
//	ncd_draw_number(gp->ibuffer[0], gp->theme->font[0], gp->theme->flag[CSD_THEME_FLAG_BOARD0_SX], gp->theme->flag[CSD_THEME_FLAG_BOARD0_SY], gp->player[0].score, 5, frame);
//	ncd_draw_number(gp->ibuffer[0], gp->theme->font[0], gp->theme->flag[CSD_THEME_FLAG_BOARD0_BX], gp->theme->flag[CSD_THEME_FLAG_BOARD0_BY], gp->player[0].destroyed, 4, frame);
//	ncd_draw_number(gp->ibuffer[0], gp->theme->font[0], gp->theme->flag[CSD_THEME_FLAG_BOARD0_LX], gp->theme->flag[CSD_THEME_FLAG_BOARD0_LY], gp->player[0].level + 1, 2, frame);
}

void csd_game_render(CSD_GAME * gp)
{
	int i;
//	int cx = 0, cy = 0, cr = 640.0, cb = 480.0;
	
	t3f_draw_animation(gp->stage->animation[CSD_THEME_ANIMATION_BACKGROUND], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0.0, 0.0, 0.0, 0);
	t3f_draw_animation(gp->stage->animation[CSD_THEME_ANIMATION_PLAYGROUND], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, gp->stage->playground_x[0], gp->stage->playground_y[0], 0.0, 0);

	for(i = 0; i < gp->players; i++)
	{
		csd_game_player_render(gp, i);
	}
	
	for(i = 0; i < 64; i++)
	{
		sprite_3d_draw(&gp->sprite[i]);
	}
	al_draw_textf(gp->stage->font, t3f_color_white, 0, 0, 0, "Score: %d", gp->player[0].score);
}
