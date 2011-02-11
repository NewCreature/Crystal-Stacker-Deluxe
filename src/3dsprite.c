#include <allegro5/allegro5.h>
#include "t3f/animation.h"
#include "3dsprite.h"

void sprite_3d_create(CSD_SPRITE_3D * sprite, T3F_ANIMATION * ap, float x, float y, float z, float vx, float vy, float vz)
{
	sprite->ap = ap;
	sprite->x = x;
	sprite->y = y;
	sprite->z = z;
	sprite->gy = 0.5;
	sprite->vx = vx;
	sprite->vy = vy;
	sprite->vz = vz;
	sprite->tick = 0;
	sprite->active = true;
}

void sprite_3d_logic(CSD_SPRITE_3D * sprite)
{
	if(sprite->active)
	{
		sprite->x += sprite->vx;
		sprite->y += sprite->vy;
		sprite->z += sprite->vz;
		sprite->vy += sprite->gy;
		if(sprite->vy > 10.0)
		{
			sprite->vy = 10.0;
		}
		sprite->tick++;
	}
}

void sprite_3d_draw(CSD_SPRITE_3D * sprite)
{
	if(sprite->active)
	{
		t3f_draw_animation(sprite->ap, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), sprite->tick, sprite->x, sprite->y, sprite->z, 0);
	}
}
