#include "theme.h"
#include "game.h"
#include "player.h"
#include "core.h"

#define WILD_PROB   80
#define TRIPLE_PROB 60
#define DOUBLE_PROB 75
#define BOMB_PROB   60

/* chart used to generate a new stack */
char type_chart[5] = {1, 2, 3, 4, 5};
char chart_size = 5;

/* removes a number from the chart (helps make non-repeating stacks) */
void core_remove_from_chart(char Number)
{
    int i;

    for(i = 0; i < 5; i++)
    {
        if(type_chart[i] == Number)
        {
            chart_size--;
            break;
        }
    }
    for(i = i; i < 4; i++)
    {
        type_chart[i] = type_chart[i + 1];
    }
}

/* resets the char used to generate stacks */
void core_reset_chart(void)
{
    type_chart[0] = 1;
    type_chart[1] = 2;
    type_chart[2] = 3;
    type_chart[3] = 4;
    type_chart[4] = 5;
    chart_size = 5;
}

void csd_player_clear_board(CSD_PLAYER * pp)
{
	int i, j;
	
	for(i = 0; i < CSD_PLAYER_BOARD_H; i++)
	{
		for(j = 0; j < CSD_PLAYER_BOARD_W; j++)
		{
			pp->board.data[i][j] = 0;
		}
	}
}

void csd_player_block_generate(CSD_PLAYER_BLOCK * bp)
{
	int i;
    char CurrentType;

    core_reset_chart();

    /* generate a triple every once in a while */
    if(rand() % TRIPLE_PROB == 0)
    {
        /* triple wild rarely */
//        if(cs_options.wild_card)
//        {
            if(rand() % WILD_PROB == 0)
            {                                
                bp->data[0] = CSD_BLOCK_TYPE_WILD;
            }
            else
            {
                bp->data[0] = rand() % CSD_BLOCK_NTYPES + 1;
            }
            bp->data[1] = bp->data[0];
            bp->data[2] = bp->data[0];
//        }

        /* normally chooses a color */
//        else
//        {
//            bp->data[0] = rand() % CSD_THEME_BLOCK_NTYPES + 1;
//            bp->data[1] = bp->data[0];
//            bp->data[2] = bp->data[0];
//        }
    }

    /* generate a bomb every once in a while */
//    else if((rand() % BOMB_PROB == 0 && cs_options.bomb))
    else if((rand() % BOMB_PROB == 0))
    {
		csd_game_add_player_message(bp->id, CSD_MESSAGE_SCROLL, "Here comes a BOMB!");
        bp->data[0] = CSD_BLOCK_TYPE_BOMB;
        bp->data[1] = CSD_BLOCK_TYPE_BOMB;
        bp->data[2] = CSD_BLOCK_TYPE_BOMB;
    }

    /* otherwise generate a normal stack */
    else
    {
        for(i = 0; i < CSD_PLAYER_BLOCK_H; i++)
        {
            /* wild cards every once in a while */
            if(rand() % WILD_PROB == 0)
            {
                bp->data[i] = CSD_BLOCK_TYPE_WILD;
            }

            /* regular crystals most of the time */
            else
            {
                CurrentType = type_chart[rand() % chart_size];

                /* allow more than one of the same color sometimes */
                if(rand() % DOUBLE_PROB != 0)
                {
                    core_remove_from_chart(CurrentType);
                }
                bp->data[i] = CurrentType;
            }
        }
    }
}

void csd_player_block_copy(CSD_PLAYER_BLOCK * dest, CSD_PLAYER_BLOCK * source)
{
	int i;
	
	for(i = 0; i < CSD_PLAYER_BLOCK_H; i++)
	{
		dest->data[i] = source->data[i];
	}
	dest->type = source->type;
}

bool csd_player_board_full(CSD_PLAYER * pp)
{
	int i;
	
	for(i = 0; i < pp->board.width; i++)
	{
		if(pp->board.data[2][i])
		{
			return true;
		}
	}
	return false;
}

void csd_player_init(CSD_PLAYER * pp, int level)
{
	csd_player_clear_board(pp);
	csd_player_block_generate(&pp->block);
	pp->block.state = CSD_PLAYER_BLOCK_STATE_ON_BOARD;
	csd_player_block_generate(&pp->block_preview);
	pp->block_preview.state = CSD_PLAYER_BLOCK_STATE_ON_PREVIEW;
	pp->score = 0;
	pp->level = level;
	pp->active = 1;
	pp->lost = false;
}
