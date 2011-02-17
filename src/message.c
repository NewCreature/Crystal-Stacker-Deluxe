#include "message.h"

void csd_clear_message_queue(CSD_MESSAGE_QUEUE * qp)
{
	qp->messages = 0;
}

bool csd_queue_message(CSD_MESSAGE_QUEUE * qp, int type, const char * message, int pos)
{
	if(qp->messages < CSD_MAX_MESSAGES)
	{
		qp->message[qp->messages].type = type;
		strcpy(qp->message[qp->messages].text, message);
		qp->message[qp->messages].pos = pos;
		qp->messages++;
	}
	return false;
}

static void csd_remove_message(CSD_MESSAGE_QUEUE * qp)
{
	int i;
	
	if(qp->messages > 0)
	{
		for(i = 0; i < qp->messages; i++)
		{
			memcpy(&qp->message[i], &qp->message[i + 1], sizeof(CSD_MESSAGE));
		}
		qp->messages--;
	}
}

/* move current message until the position of the end is less than 0 */
void csd_process_message_queue(CSD_MESSAGE_QUEUE * qp, ALLEGRO_FONT * fp)
{
	int w;
	
	if(qp->messages > 0)
	{
		switch(qp->message[0].type)
		{
			case CSD_MESSAGE_SCROLL:
			{
				w = al_get_text_width(fp, qp->message[0].text);
				qp->message[0].pos -= 4;
				if(qp->message[0].pos + w < 0)
				{
					csd_remove_message(qp);
				}
				break;
			}
			case CSD_MESSAGE_FLASH:
			{
				qp->message[0].pos -= 4;
				if(qp->message[0].pos <= 0)
				{
					csd_remove_message(qp);
				}
				break;
			}
		}
	}
}

void csd_render_message_queue(CSD_MESSAGE_QUEUE * qp, ALLEGRO_FONT * fp, float x, float y, int smooth)
{
	int w;
	
	if(qp->messages > 0)
	{
		switch(qp->message[0].type)
		{
			case CSD_MESSAGE_SCROLL:
			{
				if(smooth)
				{
					al_draw_text(fp, t3f_color_white, x + (float)qp->message[0].pos, y, 0, qp->message[0].text);
				}
				else
				{
					w = al_get_text_width(fp, "A");
					al_draw_text(fp, t3f_color_white, x + (float)(qp->message[0].pos / w) * w, y, 0, qp->message[0].text);
				}
				break;
			}
			case CSD_MESSAGE_FLASH:
			{
				if(qp->message[0].pos % 60 < 30)
				{
					al_draw_text(fp, t3f_color_white, x, y, 0, qp->message[0].text);
				}
				break;
			}
		}
	}
}
