/* the core game engine stuff */

#include "theme.h"
#include "player.h"
#include "game.h"
#include "3dsprite.h"
#include "core.h"

extern CSD_GAME csd_game;

/* deletes block from specified array, moving all blocks downward */
void core_delete_block(CSD_PLAYER_BOARD * bp, int boardx, int boardy)
{
    int i;

    /* generate effect block */
	for(i = 0; i < 64; i++)
	{
		if(!csd_game.sprite[i].active)
		{
			sprite_3d_create(&csd_game.sprite[i], csd_game.stage.crystal_animation[(int)csd_game.player[0].board.data[boardy][boardx]], csd_game.stage.layout[0].playground.x + boardx * csd_game.stage.crystal_animation[0]->frame[0]->width, csd_game.stage.layout[0].playground.y + (boardy - csd_game.stage.stack_height) * csd_game.stage.crystal_animation[0]->frame[0]->height, 20, 0.0, rand() % 2 - 1, -(8 + rand() % 3) * 2);
			break;
		}
	}

    /* delete crystal and move above crystals down */
    for(i = boardy; i > 0; i--)
    {
        bp->data[i][boardx] = bp->data[i - 1][boardx];
    }
    bp->data[0][boardx] = 0;
}

//void core_replace_block(CS_GAME * gp, CS_THEME * tp, int boardx, int boardy, int Block)
//{
//    gp->board[boardy * tp->board_w + boardx] = Block;
//}

/* insert a row of solid blocks on board */
void core_insert_solid_row(CSD_PLAYER_BOARD * bp)
{
    int i, j;

    /* move blocks up to make room for solid blocks */
    for(i = 0; i < bp->height - 1; i++)
    {
        for(j = 0; j < bp->width; j++)
        {
            bp->data[i][j] = bp->data[i + 1][j];
            bp->flag[i][j] = bp->flag[i + 1][j];
        }
    }

    /* make bottom row solid */
    for(i = 0; i < bp->width; i++)
    {
        bp->data[bp->height - 1][i] = CSD_BLOCK_TYPE_SOLID;
        bp->flag[bp->height - 1][i] = 0;
    }
}

/* marks run that goes up-right */
void core_mark_runs(CSD_PLAYER_BOARD * bp, int boardx, int boardy, int dx, int dy, int runlength)
{
    int i;

    for(i = 0; i < runlength; i++)
    {
        bp->flag[(boardy + dy * i)][(boardx + dx * i)] = 1;
    }

    /* when this is called, we know there are runs to erase */
    bp->state = CSD_PLAYER_BOARD_STATE_RUNS;
}

/* finds run that goes up-right */
int core_find_runs(CSD_PLAYER_BOARD * bp, int boardx, int boardy, int dx, int dy)
{
    int i;
    char RunLength = 0;
    int startx = boardx;
    int starty = boardy;

    /* while we are on wild cards, increase the run length */
    while(bp->data[starty][startx] == CSD_BLOCK_TYPE_WILD && starty >= 0 && starty < bp->height && startx >= 0 && startx < bp->width)
    {
        RunLength++;
        startx += dx;
        starty += dy;
    }

    /* now we take the first non-wild color and use that as the match color */
    for(i = 0; dx * i + startx < bp->width && dx * i + startx >= 0 && starty + dy * i >= 0 && starty + dy * i < bp->height && ((bp->data[(starty + dy * i)][startx + dx * i] == bp->data[starty][startx]) || (bp->data[(starty + dy * i)][startx + dx * i] == CSD_BLOCK_TYPE_WILD)) && bp->data[(starty + dy * i)][startx + dx * i] != 0 && bp->data[(starty + dy * i)][startx + dx * i] != CSD_BLOCK_TYPE_SOLID && bp->data[(starty + dy * i)][startx + dx * i] != CSD_BLOCK_TYPE_BOMB; i++)
    {
        RunLength++;
    }

    /* if our run is long enough, mark the runs for destruction */
    if(RunLength >= 3)
    {
        core_mark_runs(bp, boardx, boardy, dx, dy, RunLength);
    }

    return RunLength;
}

/* marks bombed blocks */
void core_mark_runs_bomb(CSD_PLAYER_BOARD * bp, int x, int y)
{
    int bombed;
    int i, j;

    /* see which color we are bombing */
    bombed = bp->data[y][x];

    /* do this when bombing a wild! */
    if(bombed == CSD_BLOCK_TYPE_WILD)
    {
//        queue_message(&gp->messages, 2, "            Mass Destruction!");
        /* scan board for blocks to bomb */
        for(i = 0; i < bp->height; i++)
        {
	        for(j = 0; j < bp->width; j++)
        	{
            	if(bp->data[i][j])
            	{
	                bp->flag[i][j] = 1;
				    bp->state = CSD_PLAYER_BOARD_STATE_RUNS;
            	}
        	}
        }
    }

    /* do this when bombing a solid */
    else if(bombed == CSD_BLOCK_TYPE_SOLID)
    {
//        queue_message(&gp->messages, 2, "            Can't destroy those!");
    }

    /* otherwise, do this */
    else
    {
//        queue_message(&gp->messages, 2, "            Destruction!");
        /* scan board for blocks to bomb */
        for(i = 0; i < bp->height; i++)
        {
	        for(j = 0; j < bp->width; j++)
	        {
	            if(bp->data[i][j] == bombed)
            	{
	                bp->flag[i][j] = 1;
				    bp->state = CSD_PLAYER_BOARD_STATE_RUNS;
            	}
        	}
        }
    }
}

void core_board_logic(CSD_PLAYER_BOARD * bp)
{
	switch(bp->state)
	{
		case CSD_PLAYER_BOARD_STATE_NORMAL:
		{
			break;
		}
		case CSD_PLAYER_BOARD_STATE_RUNS:
		{
			break;
		}
	}
}

/* deletes all marked blocks */
int core_delete_runs(CSD_PLAYER_BOARD * bp)
{
    int i, j;
    int TotalRemoved = 0;

    /* search entire board */
    for(i = 0; i < bp->height; i++)
    {
        for(j = 0; j < bp->width; j++)
        {
            if(bp->flag[i][j] && bp->data[i][j])
            {
                core_delete_block(bp, j, i);
                TotalRemoved++;
                bp->flag[i][j] = 0;
            }
        }
    }

    return TotalRemoved;
}

/* search for runs */
int core_find_all_runs(CSD_PLAYER_BOARD * bp)
{
    int i, j;
    int runs = 0;

    /* set runs_left off (the procedures called below will set it properly */
//    bp->state = CSD_PLAYER_BOARD_STATE_NORMAL;

    /* search the board */
    for(i = 0; i < bp->height; i++)
    {
        for(j = 0; j < bp->width; j++)
        {
            /* look in every possible direction */
            if(core_find_runs(bp, j, i, 1, 0) >= 3)
            {
	            runs = 1;
            }
            if(core_find_runs(bp, j, i, 0, 1) >= 3)
            {
	            runs = 1;
            }
            if(core_find_runs(bp, j, i, 1, -1) >= 3)
            {
	            runs = 1;
            }
            if(core_find_runs(bp, j, i, 1, 1) >= 3)
            {
	            runs = 1;
            }
        }
    }
    return runs;
}
