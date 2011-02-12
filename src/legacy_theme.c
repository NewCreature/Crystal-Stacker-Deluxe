#include "t3f/t3f.h"
#include "t3f/music.h"
#include "theme.h"

T3F_ANIMATION * bitmap2ani(ALLEGRO_BITMAP * bp)
{
	T3F_ANIMATION * ap;

	ap = t3f_create_animation();
	t3f_animation_add_bitmap(ap, bp);
	t3f_animation_add_frame(ap, 0, 0, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), 0, 1);

	return ap;
}

T3F_ANIMATION * csd_legacy_load_ani_fp(ALLEGRO_FILE * fp, ALLEGRO_COLOR pal[256])
{
    T3F_ANIMATION * ap;
    char header[4];
    int i, j, k;
    int w, h, f, d;
    ALLEGRO_STATE old_state;

    /* check format */
    al_fread(fp, header, 4);
    if(header[0] != 'A' || header[1] != 'N' || header[2] != 'I' || header[3] != 21)
    {
        return NULL;
    }

    ap = t3f_create_animation();

    /* load header */
    w = al_fread32le(fp);
    h = al_fread32le(fp);
    f = al_fread32le(fp);
    d = al_fread32le(fp);
    
    /* load palette data */
    j = al_fgetc(fp);
    if(j)
    {
        if(pal != NULL)
        {
            for(i = 0; i < 256; i++)
            {
                al_fgetc(fp);
                al_fgetc(fp);
                al_fgetc(fp);
            }
        }
        else
        {
            for(i = 0; i < 256 * 3; i++)
            {
                al_fgetc(fp);
            }
        }
    }

    /* load animation data */
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	ap->bitmaps = f;
    for(i = 0; i < f; i++)
    {
        ap->bitmap[i] = al_create_bitmap(w, h);
        al_lock_bitmap(ap->bitmap[i], al_get_bitmap_format(ap->bitmap[i]), ALLEGRO_LOCK_WRITEONLY);
        al_set_target_bitmap(ap->bitmap[i]);
        for(j = 0; j < h; j++)
        {
            for(k = 0; k < w; k++)
            {
	            al_put_pixel(k, j, pal[al_fgetc(fp)]);
            }
        }
        al_unlock_bitmap(ap->bitmap[i]);
        t3f_animation_add_frame(ap, i, 0, 0, 0, w * 2, h * 2, 0, d > 0 ? d : 1);
    }
	al_restore_state(&old_state);

    return ap;
}

ALLEGRO_FONT * csd_legacy_load_ncdfont_fp(ALLEGRO_FILE * fp, ALLEGRO_COLOR pal[256])
{
    int i, j, k, c;
    char Header[4];
    ALLEGRO_BITMAP * bp;
    ALLEGRO_BITMAP * cbp;
    ALLEGRO_FONT * font;
    ALLEGRO_STATE old_state;
    int fw, fh;
	int ranges[] = {0, 255};

    /* read header */
    al_fread(fp, &Header, 4);

    /* if header is wrong, return 0 */
    if(Header[0] != 'M' || Header[1] != 'F' || Header[2] != 'T' || Header[3] != 21)
    {
        return NULL;
    }

    /* read font dimensions */
    fw = al_fgetc(fp);
    fh = al_fgetc(fp);

	/* create font bitmap */
	bp = al_create_bitmap((fw + 1) * 16 + 1, (fh + 1) * 16 + 1);
	if(!bp)
	{
		return NULL;
	}
	
	/* create character bitmap */
	cbp = al_create_bitmap(fw, fh);
	if(!cbp)
	{
		return NULL;
	}
	
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	
	/* make yellow border to divide characters */
	al_set_target_bitmap(bp);
	al_clear_to_color(al_map_rgb_f(1.0, 1.0, 0.0));
	
    /* read font data to file */
    for(i = 0; i < 256; i++)
    {
		al_set_target_bitmap(cbp);
        for(j = 0; j < fh; j++)
        {
            for(k = 0; k < fw; k++)
            {
				c = al_fgetc(fp);
				if(c > 0)
				{
					al_put_pixel(k, j, pal[c]);
				}
				else
				{
					al_put_pixel(k, j, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
				}
            }
        }
        al_set_target_bitmap(bp);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
        al_draw_bitmap(cbp, 1 + (i % 16) * (fw + 1), 1 + (i / 16) * (fh + 1), 0);
    }
	al_destroy_bitmap(cbp);
    al_restore_state(&old_state);

	font = al_grab_font_from_bitmap(bp, 1, ranges);
	al_destroy_bitmap(bp);
	
    return font;
}

/* loads a legacy theme */
CSD_THEME * csd_load_legacy_theme(const char * fn)
{
	CSD_THEME * tp;
	
	tp = malloc(sizeof(CSD_THEME));
	if(!tp)
	{
		return NULL;
	}
	tp->path = al_create_path(fn);
	if(!tp->path)
	{
		return NULL;
	}
	tp->config = al_create_config();
	if(!tp->config)
	{
		return NULL;
	}
	al_set_config_value(tp->config, "Settings", "Legacy", "true");
	return tp;
}

bool csd_load_legacy_stage(CSD_THEME * tp, CSD_STAGE * sp)
{
    ALLEGRO_FILE * file;
    int i, j, k, c;
    char TempBit;
    char header[16] = {0};
    int rw, rh, r, g, b;
    int bw, bh, tw, th, mw, mh;
    float a = 1.0;
    int block_chart[16] = {9, 0, 1, 2, 3, 4, 5, 6};
	ALLEGRO_COLOR palette[256];
    ALLEGRO_BITMAP * block_image;
    ALLEGRO_BITMAP * block_flash_image;
    ALLEGRO_BITMAP * tile_image;
    ALLEGRO_BITMAP * bp;
    ALLEGRO_STATE old_state;
    ALLEGRO_PATH * music_path = NULL;

    file = al_fopen(al_path_cstr(tp->path, '/'), "r");
    if(file == NULL)
    {
        return false;
    }

    /* read the header and see if it is a theme file */
    al_fread(file, header, 4);
    if(header[0] != 'C' || header[1] != 'T' || header[2] != 'H' || (header[3] != 21 && header[3] != 22))
    {
        al_fclose(file);
        return false;
    }
    
    al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);

    /* video card stuff */
    al_fgetc(file);
    rw = al_fread16le(file);
    rh = al_fread16le(file);

    /* load the palette */
    for(i = 0; i < 256; i++)
    {
        r = al_fgetc(file);
        g = al_fgetc(file);
        b = al_fgetc(file);
        palette[i] = al_map_rgba_f((float)r / 63.0, (float)g / 63.0, (float)b / 63.0, a);
    }

    /* game board stuff */
    sp->board_width = al_fgetc(file);
    sp->board_height = al_fgetc(file);
    for(i = 0; i < 2; i++)
    {
        sp->layout[i].playground.x = al_fread16le(file);
        sp->layout[i].playground.y = al_fread16le(file);
        sp->layout[i].score.x = al_fread16le(file);
        sp->layout[i].score.y = al_fread16le(file);
        sp->layout[i].preview.x = al_fread16le(file);
        sp->layout[i].preview.y = al_fread16le(file);
        sp->layout[i].level.x = al_fread16le(file);
        sp->layout[i].level.y = al_fread16le(file);
        sp->layout[i].block.x = al_fread16le(file);
        sp->layout[i].block.y = al_fread16le(file);
        sp->layout[i].message.x = al_fread16le(file);
        sp->layout[i].message.y = al_fread16le(file);
    }

    sp->time.x = al_fread16le(file);
    sp->time.y = al_fread16le(file);
    sp->high_score.x = al_fread16le(file);
    sp->high_score.y = al_fread16le(file);
    sp->best_time.x = al_fread16le(file);
    sp->best_time.y = al_fread16le(file);
    sp->message.x = al_fread16le(file);
    sp->message.y = al_fread16le(file);

    /* block stuff */
    if(header[3] == 21)
    {
		printf("old blocks\n");
	    bw = al_fgetc(file);
	    bh = al_fgetc(file);
	    for(i = 0; i < 7; i++)
	    {
			block_image = al_create_bitmap(bw, bh);
			block_flash_image = al_create_bitmap(bw, bh);
        	for(j = 0; j < bh; j++)
        	{
	            for(k = 0; k < bw; k++)
            	{
					al_set_target_bitmap(block_image);
					c = al_fgetc(file);
					if(c < 256)
					{
						al_put_pixel(k, j, palette[c]);
					}
					al_set_target_bitmap(block_flash_image);
					c = al_fgetc(file);
					if(c < 256)
					{
						al_put_pixel(k, j, palette[c]);
					}
            	}
        	}
        	sp->crystal_animation[i] = bitmap2ani(block_image);
        	sp->fcrystal_animation[i] = bitmap2ani(block_flash_image);
    	}
    }
    else if(header[3] == 22)
    {
	    for(i = 0; i < 7; i++)
	    {
        	sp->crystal_animation[i] = csd_legacy_load_ani_fp(file, palette);
        	sp->fcrystal_animation[i] = csd_legacy_load_ani_fp(file, palette);
    	}
    }

    /* tile stuff (old) */
    if(header[3] == 21)
    {
	    tw = al_fgetc(file);
	    th = al_fgetc(file);
	    if(tw > 0 && th > 0)
	    {
		    tile_image = al_create_bitmap(tw, th);
        	for(i = 0; i < 40; i++)
        	{
	            for(j = 0; j < th; j++)
            	{
	                for(k = 0; k < tw; k++)
                	{
//	                    tile_image->line[j][k] = al_fgetc(file);
	                    al_fgetc(file);
                	}
            	}
//	            tp->tile_image[i] = bitmap2ani(tile_image);
        	}
        	al_destroy_bitmap(tile_image);
    	}
    	else
    	{
			bp = al_create_bitmap(rw, rh);
			al_set_target_bitmap(bp);
	        for(i = 0; i < rh; i++)
        	{
	            for(j = 0; j < rw; j++)
            	{
					al_put_pixel(j, i, palette[al_fgetc(file)]);
            	}
        	}
        	sp->animation[CSD_THEME_ANIMATION_BACKGROUND] = bitmap2ani(bp);
    	}
    }

    /* tile stuff (new) */
    else
    {
	    tw = al_fgetc(file);
	    th = al_fgetc(file);
	    if(tw > 0 && th > 0)
	    {
        	for(i = 0; i < 40; i++)
        	{
	            csd_legacy_load_ani_fp(file, palette);
        	}
    	}
    	else
    	{
			bp = al_create_bitmap(rw, rh);
			al_set_target_bitmap(bp);
	        for(i = 0; i < rh; i++)
        	{
	            for(j = 0; j < rw; j++)
            	{
					al_put_pixel(j, i, palette[al_fgetc(file)]);
            	}
        	}
    	}
    }

    /* font stuff */
    sp->font = csd_legacy_load_ncdfont_fp(file, palette);

    /* map stuff */
    mw = al_fgetc(file);
    mh = al_fgetc(file);
    if(mw > 0 && mh > 0)
    {
//        tp->map = malloc(tp->map_w * tp->map_h);
        for(i = 0; i < mh; i++)
        {
            for(j = 0; j < mw; j++)
            {
//                tp->map[i * tp->map_w + j] = al_fgetc(file);
                al_fgetc(file);
            }
        }
    }
    al_restore_state(&old_state);

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_DROP] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_DROP] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_ROTATE_UP] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_ROTATE_UP] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_ROTATE_DOWN] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_ROTATE_DOWN] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_MOVE] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_MOVE] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_MOVE_FAIL] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_MOVE_FAIL] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_LAND] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_LAND] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_MATCHES] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_MATCHES] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_DELETE] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_DELETE] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_LEVEL] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_LEVEL] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_PAUSE] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_PAUSE] = NULL;
    }

    TempBit = al_fgetc(file);
    if(TempBit == 1)
    {
        sp->sample[CSD_THEME_SAMPLE_ATTACK] = al_load_sample_f(file, ".wav");
    }
    else
    {
        sp->sample[CSD_THEME_SAMPLE_ATTACK] = NULL;
    }

	sp->stack_height = 3;
	sp->block_types = 5;

    /* close the file */
    al_fclose(file);
    
    music_path = al_clone_path(tp->path);
    al_set_path_extension(music_path, ".xm");
    t3f_play_music(al_path_cstr(music_path, '/'));
    al_destroy_path(music_path);

    return 1;
}