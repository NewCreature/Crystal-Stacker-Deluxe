#ifndef CSD_MESSAGE_H
#define CSD_MESSAGE_H

#include "t3f/t3f.h"

#define CSD_MAX_MESSAGES  16

#define CSD_MESSAGE_SCROLL 0
#define CSD_MESSAGE_FLASH  1

typedef struct
{

	int type;
	char text[256];
	int pos;

} CSD_MESSAGE;

typedef struct
{
	
	CSD_MESSAGE message[CSD_MAX_MESSAGES];
	int messages;
	
} CSD_MESSAGE_QUEUE;

void csd_clear_message_queue(CSD_MESSAGE_QUEUE * qp);
bool csd_queue_message(CSD_MESSAGE_QUEUE * qp, int type, const char * message, int pos);
void csd_process_message_queue(CSD_MESSAGE_QUEUE * qp, ALLEGRO_FONT * fp);
void csd_render_message_queue(CSD_MESSAGE_QUEUE * qp, ALLEGRO_FONT * fp, float x, float y, int smooth);

#endif
