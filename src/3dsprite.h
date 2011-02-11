#ifndef CSD_SPRITE3D_H
#define CSD_SPRITE3D_H

#include <allegro5/allegro5.h>
#include "../../../Dev/T3F/t3f/animation.h"

typedef struct
{
	
	T3F_ANIMATION * ap;
	float x, y, z, gy;
	float vx, vy, vz;
	int tick;
	
	bool active;

} CSD_SPRITE_3D;

void sprite_3d_create(CSD_SPRITE_3D * sprite, T3F_ANIMATION * ap, float x, float y, float z, float vx, float vy, float vz);
void sprite_3d_logic(CSD_SPRITE_3D * sprite);
void sprite_3d_draw(CSD_SPRITE_3D * sprite);

#endif
