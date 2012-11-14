#include "eat.h"

#define BIG_WIDTH (232-84)
#define BIG_HEIGHT (194-137)
#define BIG_MODIFIER 4
#define START_AREA (149-137)

extern int radar_position;

int radar_xstart = 80, radar_ystart = 133, PBianca = 0;
int radarlines[] = {/* 84, 137, */ 232, 137, 232, 194, 84, 194, 84, 137};
int area1[] = {/* 84, 152, */ 104, 149, 104, 182, 84, 182};
int area2[] = {/* 232, 152, */ 212, 149, 212, 182, 232, 182};

BOOL big = TRUE;
BOOL small_dot;

void HandleRadarBig(void)
{
    uint8_t *bm = main_bitmap + radar_xstart + (radar_ystart*bitmap_width);
    uint8_t *bm2, *bmt;
    uint8_t *main_bitmap_start = main_bitmap + bitmap_width;
    uint8_t *main_bitmap_end = main_bitmap + bitmap_width*(bitmap_height-1);
    int j, c0 = p->team[0]->MarkerRed, c1 = p->team[1]->MarkerRed;

    polydraw(Pens[P_NERO], radar_xstart+BIG_MODIFIER, radar_ystart+BIG_MODIFIER, 4, radarlines);
    polydraw(Pens[P_NERO], radar_xstart+BIG_MODIFIER, radar_ystart+(BIG_MODIFIER+START_AREA), 3, area1);
    polydraw(Pens[P_NERO], radar_xstart+(BIG_MODIFIER+BIG_WIDTH), radar_ystart+(BIG_MODIFIER+START_AREA), 3, area2);
    draw(Pens[P_NERO], radar_xstart+BIG_MODIFIER+(BIG_WIDTH/2), radar_ystart+BIG_MODIFIER, radar_xstart+BIG_MODIFIER+(BIG_WIDTH/2), radar_ystart+BIG_MODIFIER+BIG_HEIGHT);

    if(small_dot)
    {
        for(j = 0; j < 10; j++)
        {
            bmt = bm+(p->team[0]->players[j].world_x >> 6)+(p->team[0]->players[j].world_y >> 6)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
                *bmt = c0;

            bmt = bm+(p->team[1]->players[j].world_x >> 6)+(p->team[1]->players[j].world_y >> 6)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
                *bmt = c1;
        }
    }
    else
    {
        for(j = 0; j < 10; j++)
        {
            bmt = bm+(p->team[0]->players[j].world_x >> 6)+(p->team[0]->players[j].world_y >> 6)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
            {
                *bmt = c0;
                bmt++;
                *bmt = c0;
                bmt += bitmap_width;
                *bmt = c0;
                bmt--;
                *bmt = c0;
            }

            bmt = bm+(p->team[1]->players[j].world_x >> 6)+(p->team[1]->players[j].world_y >> 6)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
            {
                *bmt = c1;
                bmt++;
                *bmt = c1;
                bmt += bitmap_width;
                *bmt = c1;
                bmt--;
                *bmt = c1;
            }
        }
    }

// Faccio 4 punti per l'omino attivo

    if(small_dot)
    {
        bm2 = bm+(p->team[0]->attivo->world_x >> 6)+(p->team[0]->attivo->world_y >> 6)*bitmap_width+1;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2--;
            *bm2 = c0;
        }

        bm2 = bm+(p->team[1]->attivo->world_x >> 6)+(p->team[1]->attivo->world_y >> 6)*bitmap_width+1;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c1;
            bm2 += bitmap_width;
            *bm2 = c1;
            bm2--;
            *bm2 = c1;
        }
    }
    else
    {
        bm2 = bm+(p->team[0]->attivo->world_x >> 6)+(p->team[0]->attivo->world_y >> 6)*bitmap_width+2;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2 -= (bitmap_width+bitmap_width+2);
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
        }

        bm2 = bm+(p->team[1]->attivo->world_x >> 6)+(p->team[1]->attivo->world_y >> 6)*bitmap_width+2;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2 -= (bitmap_width+bitmap_width+2);
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
        }
    }

    PBianca ^= 1;

    bm2 = bm+(pl->world_x >> 6)+((pl->world_y >> 6)-1)*bitmap_width;
    
    if(bm2 > main_bitmap_start && bm2 < main_bitmap_end)
    {
        if(small_dot)
        {
            *bm2= (UBYTE) (PBianca ? Pens[P_BIANCO] : Pens[P_NERO]); // E' piccola, la tengo un pixel sopra
        }
        else
        {
            UBYTE b = (UBYTE)(PBianca ? Pens[P_BIANCO] : Pens[P_NERO]); // E' piccola, la tengo un pixel sopra

            *bm2 = b;
            bm2++;
            *bm2 = b;
            bm2 += bitmap_width;
            *bm2 = b;
            bm2--;
            *bm2 = b;
        }
    }

    if(arcade)
    {
        for(j = 0; j < MAX_ARCADE_ON_FIELD; j++)
            if(bonus[j]->world_x > 0)
            {
                bm2 = bm+(bonus[j]->world_x >> 6)+(bonus[j]->world_y >> 6)*bitmap_width;
            
                *bm2 = Pens[P_GIALLO];

                if(!small_dot)
                {
                    bm2++;
                    *bm2 = Pens[P_GIALLO];
                    bm2 += bitmap_width;
                    *bm2 = Pens[P_GIALLO];
                    bm2--;
                    *bm2 = Pens[P_GIALLO];
                }
            }
    }
}

void HandleRadarLittle(void)
{
    uint8_t *bm = main_bitmap+radar_xstart+(radar_ystart*bitmap_width);
    uint8_t *bm2, *bmt;
    uint8_t *main_bitmap_start = main_bitmap+bitmap_width;
    uint8_t *main_bitmap_end = main_bitmap+bitmap_width*(bitmap_height-1);
    int j, c0 = p->team[0]->MarkerRed, c1 = p->team[1]->MarkerRed;

    polydraw(Pens[P_NERO], radar_xstart+(BIG_MODIFIER/2), radar_ystart+(BIG_MODIFIER/2), 4, radarlines);
    polydraw(Pens[P_NERO], radar_xstart+(BIG_MODIFIER/2), radar_ystart+((BIG_MODIFIER+START_AREA)/2), 3, area1);
    polydraw(Pens[P_NERO], radar_xstart+((BIG_MODIFIER+BIG_WIDTH)/2), radar_ystart+((BIG_MODIFIER+START_AREA)/2), 3, area2);
    draw(Pens[P_NERO], radar_xstart+(BIG_MODIFIER/2)+(BIG_WIDTH/4), radar_ystart+(BIG_MODIFIER/2), radar_xstart+(BIG_MODIFIER/2)+(BIG_WIDTH/4), radar_ystart+(BIG_MODIFIER+BIG_HEIGHT)/2);

    if(small_dot)
    {
        for(j = 0; j < 10; j++)
        {
            bmt = bm+(p->team[0]->players[j].world_x >> 7)+(p->team[0]->players[j].world_y >> 7)*bitmap_width;
            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
                *bmt = c0;

            bmt = bm+(p->team[1]->players[j].world_x >> 7)+(p->team[1]->players[j].world_y >> 7)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
                *bmt = c1;
        }
    }
    else
    {
        for(j = 0; j < 10; j++)
        {
            bmt = bm+(p->team[0]->players[j].world_x >> 7)+(p->team[0]->players[j].world_y >> 7)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
            {
                *bmt = c0;
                bmt++;
                *bmt = c0;
                bmt += bitmap_width;
                *bmt = c0;
                bmt--;
                *bmt = c0;
            }

            bmt = bm+(p->team[1]->players[j].world_x >> 7)+(p->team[1]->players[j].world_y >> 7)*bitmap_width;

            if(bmt < main_bitmap_end && bmt > main_bitmap_start)
            {
                *bmt = c1;
                bmt++;
                *bmt = c1;
                bmt += bitmap_width;
                *bmt = c1;
                bmt--;
                *bmt = c1;
            }
        }
    }

    if(small_dot)
    {
        bm2 = bm+(p->team[0]->attivo->world_x >> 7)+(p->team[0]->attivo->world_y >> 7)*bitmap_width+1;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2--;
            *bm2 = c0;
        }

        bm2 = bm+(p->team[1]->attivo->world_x >> 7)+(p->team[1]->attivo->world_y >> 7)*bitmap_width+1;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c1;
            bm2 += bitmap_width;
            *bm2 = c1;
            bm2--;
            *bm2 = c1;
        }
    }
    else
    {
        bm2 = bm+(p->team[0]->attivo->world_x >> 7)+(p->team[0]->attivo->world_y >> 7)*bitmap_width+2;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2 -= (bitmap_width+bitmap_width+2);
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
        }

        bm2 = bm+(p->team[1]->attivo->world_x >> 7)+(p->team[1]->attivo->world_y >> 7)*bitmap_width+2;

        if(bm2 < main_bitmap_end && bm2 > main_bitmap_start)
        {
            *bm2 = c0;
            bm2 += bitmap_width;
            *bm2 = c0;
            bm2 -= (bitmap_width+bitmap_width+2);
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
            bm2++;
            *bm2 = c0;
        }
    }

    PBianca ^= 1;

    bm2 = bm+(pl->world_x >> 7)+((pl->world_y >> 7)-1)*bitmap_width;
    
    if(bm2 > main_bitmap_start && bm2 < main_bitmap_end)
    {
        if(small_dot)
        {
            *bm2= (UBYTE) (PBianca ? Pens[P_BIANCO] : Pens[P_NERO]); // E' piccola, la tengo un pixel sopra
        }
        else
        {
            UBYTE b = (BYTE)(PBianca ? Pens[P_BIANCO] : Pens[P_NERO]); // E' piccola, la tengo un pixel sopra

            *bm2 = b;
            bm2++;
            *bm2 = b;
            bm2 += bitmap_width;
            *bm2 = b;
            bm2--;
            *bm2 = b;
        }
    }

    if(arcade)
    {
        for(j = 0; j < MAX_ARCADE_ON_FIELD; j++)
            if(bonus[j]->world_x > 0)
            {
                bm2 = bm+(bonus[j]->world_x >> 7)+(bonus[j]->world_y >> 7)*bitmap_width;
                
                *bm2 = Pens[P_GIALLO];

                if(!small_dot)
                {
                    bm2++;
                    *bm2 = Pens[P_GIALLO];
                    bm2 += bitmap_width;
                    *bm2 = Pens[P_GIALLO];
                    bm2--;
                    *bm2 = Pens[P_GIALLO];
                }
            }
    }
}

void ResizeRadar(void)
{
    WORD old_x = radar_xstart, old_y = radar_ystart, i;
    BOOL changed = FALSE;

    old_x += (BIG_MODIFIER/2);
    old_y += (BIG_MODIFIER/2);

    if(big)
    {
        old_x += (BIG_MODIFIER/2);
        old_y += (BIG_MODIFIER/2);
    }

    if(radar_position > 5)
    {
        radar_position -= 6;
        changed = TRUE;

        if(big)
        {
            HandleRadar = HandleRadarLittle;
            big = FALSE;
        }
        else
        {
            HandleRadar = HandleRadarBig;
            big = TRUE;
        }
    }

    if(radar_position < 3)
    {
        radar_ystart = 1-(BIG_MODIFIER/2);
    }
    else
    {
        radar_ystart = WINDOW_HEIGHT-( (big==TRUE) ? BIG_HEIGHT : (BIG_HEIGHT/2) ) - 2 - BIG_MODIFIER;
        radar_position -= 3;
    }

    if(radar_position==0)
    {
        radar_xstart = 1-(BIG_MODIFIER/2);
    }
    else if(radar_position==1)
    {
        radar_xstart = (WINDOW_WIDTH/2) - ( (big==TRUE) ? BIG_WIDTH : (BIG_WIDTH/2) )/2 - 1 - (BIG_MODIFIER/2);
    }
    else
    {
        radar_xstart = WINDOW_WIDTH-( (big==TRUE) ? BIG_WIDTH : (BIG_WIDTH/2) ) - 2 - BIG_MODIFIER;
    }


    for(i = 0; i < 8; i += 2)
    {
        radarlines[i] = radarlines[i]-old_x;
        radarlines[i+1] = radarlines[i+1]-old_y;

        if(i < 6)
        {
            area1[i] = area1[i]-old_x;
            area1[i+1] = area1[i+1]-old_y;
            area2[i] = area2[i]-old_x;
            area2[i+1] = area2[i+1]-old_y;

            if(big)
            {
                if(changed)
                {
                    area1[i] *= 2;
                    area1[i+1] *= 2;
                    area2[i] *= 2;
                    area2[i+1] *= 2;
                }

                area1[i] += (radar_xstart+BIG_MODIFIER);
                area1[i+1] += (radar_ystart+BIG_MODIFIER);
                area2[i] += (radar_xstart+BIG_MODIFIER);
                area2[i+1] += (radar_ystart+BIG_MODIFIER);
            }
            else
            {
                if(changed)
                {
                    area1[i] /= 2;
                    area1[i+1] /= 2;
                    area2[i] /= 2;
                    area2[i+1] /= 2;
                }

                area1[i] += (radar_xstart+(BIG_MODIFIER/2));
                area1[i+1] += (radar_ystart+(BIG_MODIFIER/2));
                area2[i] += (radar_xstart+(BIG_MODIFIER/2));
                area2[i+1] += (radar_ystart+(BIG_MODIFIER/2));
            }
        }

        if(big)
        {
            if(changed)
            {
                radarlines[i] *= 2;
                radarlines[i+1] *= 2;
            }

            radarlines[i] += (radar_xstart+BIG_MODIFIER);
            radarlines[i+1] += (radar_ystart+BIG_MODIFIER);
        }
        else
        {
            if(changed)
            {
                radarlines[i] /= 2;
                radarlines[i+1] /= 2;
            }

            radarlines[i] += (radar_xstart+(BIG_MODIFIER/2));
            radarlines[i+1] += (radar_ystart+(BIG_MODIFIER/2));
        }
    }

    if(radar_ystart != (1-(BIG_MODIFIER/2)) )
        radar_position += 3;
}
