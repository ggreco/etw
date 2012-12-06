#include "etw_locale.h"
#include "SDL.h"
#include "menu.h"

#define BFP(f,s,b) fprintf(f,s, (b) ? "on"/*-*/ : "off"/*-*/ )

extern SDL_Surface *screen;

extern char team_name[2][16];
extern void SetCurrentResolution(void);
char network_server[80]={0};

int radar_position = 7;

BOOL wb_game=FALSE,use_replay=TRUE,allow_replay=TRUE,nocpu=FALSE,arcade=FALSE,substitutions=TRUE,
    situation=FALSE,use_crowd=TRUE,use_speaker=FALSE,free_longpass=FALSE,injuries=TRUE,    
    bookings=TRUE,nopari=FALSE,id_change=FALSE,window_opened=FALSE,newchange=FALSE,
    penalties=FALSE,free_kicks=FALSE,cgxmode=FALSE,nointro=FALSE,killer=FALSE,warp=FALSE,
    music_playing=FALSE,golden_gol=FALSE,
    offside=FALSE,screen_opened=TRUE,network_game=FALSE, menu_music=TRUE;

int8_t situation_result[2]={0,0},strictness=10;

int display_id = 0, wanted_width, wanted_height;
long int situation_time = 0;
char localename[40];

void OpenMenuScreen(void)
{
    extern void OpenTheScreen();
    extern void SetTitle(const char *);

    OpenTheScreen();

    SetTitle("Eat The Whistle " ETW_VERSION);


    if(os_check_joy(0))
        sprefs_bottoni[menu[MENU_SYSTEM_PREFS].NumeroBottoni-3].Text="JOYSTICK CONFIG";
}

void load_config(FILE *f)
{
    char buffer[120];
    int value;

    if(f)
    {
        while(fgets(buffer,119,f)!=NULL)
        {
// Due giocatori?
            if(buffer[0]==';'||buffer[0]=='/')
            {
// E' un commento...
            }
            else if(!strnicmp(buffer,"server="/*-*/, 7)) {
                char *c;
                
                strncpy(network_server, buffer + 7, sizeof(network_server));

                c = strchr(network_server, '\n');

                if(c)
                    *c = 0;
            }
            else if(!strnicmp(buffer,"strictness="/*-*/,11))
            {
                if((value=atol(&buffer[11]))>0)
                    if(value>=0&&value<11)
                        strictness=value;
            }
            else if(!strnicmp(buffer,"frames="/*-*/,7))
            {
                if((value=atol(&buffer[7]))>0)
                    if(value>0)
                        framerate=1000000/value;
            }
#ifndef MOBILE_VERSION
            else if(!strnicmp(buffer,"width="/*-*/,6))
            {
                if((value=atol(&buffer[6]))>0)
                    WINDOW_WIDTH=value;
            }        
            else if(!strnicmp(buffer,"height="/*-*/,7))
            {
                if((value=atol(&buffer[7]))>0)
                    WINDOW_HEIGHT=value;
            }
#endif
            else if(!strnicmp(buffer,"nointro"/*-*/,7) )
            {
                nointro=TRUE;
            }
            else if(!strnicmp(buffer,"offside=on"/*-*/,10) )
            {
                offside=TRUE;
            }
            else if(!strnicmp(buffer,"offside=off"/*-*/,11) )
            {
                offside=FALSE;
            }
            else if(!strnicmp(buffer,"sound=off"/*-*/,9) )
            {
                no_sound=TRUE;
            }
            else if(!strnicmp(buffer,"newchange"/*-*/,9))
            {
                newchange=TRUE;
            }
            else if(!strnicmp(buffer,"scaling_y"/*-*/,9))
            {
                /* AC: il valore da utilizzare per ottenere lo scaling, e' 10.
                 * con 9, viene contato anche il carattere =.
                 */
                if((value=atol(&buffer[10]))>0)
                {
                    extern int FIXED_SCALING_WIDTH,FIXED_SCALING_HEIGHT;
                    extern char *scaling_resolutions[];
                    int i;

                    for(i=0;scaling_resolutions[i];i++)
                    {
                        if(value==atol(scaling_resolutions[i]+4))
                        {
                            extern BYTE current_scaling;

                            current_scaling=(BYTE)i;

                            FIXED_SCALING_HEIGHT=value;
                            FIXED_SCALING_WIDTH=atol(scaling_resolutions[i]);
                        }
                    }
                }
            }
            else if(!strnicmp(buffer,"scaling"/*-*/,7) )
            {
                use_gfx_scaling=TRUE;
            }
            else if(!strnicmp(buffer,"workbench"/*-*/,9) )
            {
                wb_game=TRUE;
            }
            else if(!strnicmp(buffer,"displayid="/*-*/,10) )
            {
                if((value=atol(&buffer[10]))>0)
                    display_id=value;
            }
            else if(!strnicmp(buffer,"radar="/*-*/,6) )
            {
                if((value=atol(&buffer[6]))>0)
                {
                    if(value>=0 && value <12)
                    {
                        radar_position=value;
                    }
                    else
                    {
                        detail_level&=~USA_RADAR;
                    }
                }
                else
                {
                    detail_level&=~USA_RADAR;
                }
            }
            else if(!strnicmp(buffer,"control"/*-*/,7))
            {
                if( (buffer[7]>='0') && (buffer[7]<='1') && (buffer[8]=='='))
                {
                    int i;

                    for(i=0;i<(CONTROLS-1);i++)
                    {
                        if(!strnicmp(&buffer[9],controls[i],strlen(controls[i])))
                        {
                            int team = buffer[7]-'0';
                           
                            // if joystick is not present fallback to keyboard
                            if (i < CTRL_KEY_1 && !os_check_joy(0))
                               i = CTRL_KEY_1; 
                                
                            control[team ^ 1]=i;
                            
                                
                            D(bug("Control for team %ld: %s\n"/*-*/, 
                                        team + 1,controls[i]));
                            
                            break;
                        }
                    }
                }
                else
                {
                    D(bug("Errore nella sintassi di control nella config.\n"/*-*/));    
                }
            }
            else if(!strnicmp(buffer,"time="/*-*/,5))
            {
                if((value=atol(&buffer[5]))>0)
                {
                    if(value>0&&value<=45)
                    {
                        extern char *time_options[];

                        duration=0;
                        t_l=value;

                        while(time_options[duration]&&t_l!=atol(time_options[duration]))
                            duration++;

                        if(!time_options[duration])
                        {
                            D(bug("Durata non permessa!\n"));
                            duration=1;
                        }
                    }
                }
            }
            else if(!strnicmp(buffer,"autoreplay=off"/*-*/,13))
            {
                use_replay=FALSE;
            }
            else if(!strnicmp(buffer,"nocpu"/*-*/,5))
            {
                nocpu=TRUE;
            }
            else if(!strnicmp(buffer,"result=off"/*-*/,10))
            {
                detail_level&=~USA_RISULTATO;                
            }
            else if(!strnicmp(buffer,"result=on"/*-*/,9))
            {
                detail_level|=USA_RISULTATO;
            }
            else if(!strnicmp(buffer,"crowd=off"/*-*/,9))
            {
                use_crowd=FALSE;
            }
            else if(!strnicmp(buffer,"speaker=on"/*-*/,10))
            {
                D(bug("Attivo lo speaker virtuale!\n"/*-*/));
                use_crowd=FALSE;
                use_speaker=TRUE;
            }
            else if(!strnicmp(buffer,"freepass"/*-*/,8))
            {
                free_longpass=TRUE;
            }
            else if(!strnicmp(buffer,"menumusic=on"/*-*/,12))
            {
                menu_music=TRUE;
            }
            else if(!strnicmp(buffer,"menumusic=off"/*-*/,13))
            {
                menu_music=FALSE;
            }
            else if(!strnicmp(buffer,"field_type="/*-*/,11))
            {    
                if((value=atol(&buffer[11]))>0)
                {
                    if(value>0&&value<10)
                        field_type=value;
                }
            }
            else if(!strnicmp(buffer,"field="/*-*/,6))
            {    
                if((value=atol(&buffer[6]))>0)
                {
                    if(value>0&&value<10)
                        field=value;
                }
            }
        }

        fclose(f);
    }
}

void read_menu_config(void)
{
    FILE *f;
    D(bug("Reading configuration...\n"/*-*/));

    f=fopen("etw.cfg"/*-*/,"r");

    load_config(f);

    SetCurrentResolution();
}

void write_config(char *dest)
{
    FILE *f;

    D(bug("Writing configuration...\n"/*-*/));

    if ((f = fopen(dest,"w"))) {
// Obsoleto        fprintf(f,"players=%ld\n"/*-*/,players);
// "frames=%ld",1000000/framerate  (sempre a 25)
        BFP(f,"sound=%s\n"/*-*/,!no_sound);
// Mi serve ancora per il debug...

        if(network_server[0])
            fprintf(f,"server=%s\n", network_server);

        if(nointro)
            fprintf(f,"nointro\n"/*-*/);


        if(use_gfx_scaling)
        {
            extern int FIXED_SCALING_HEIGHT;

            fprintf(f,"scaling_y=%d\n"/*-*/,FIXED_SCALING_HEIGHT);
            fprintf(f,"scaling\n"/*-*/);
        }

        if(penalties)
            fprintf(f,"penalties\n"/*-*/);
        else if(free_kicks)
            fprintf(f,"freekicks\n"/*-*/);

        if(golden_gol)
            fprintf(f,"golden\n"/*-*/);

        if(newchange)
            fprintf(f,"newchange\n"/*-*/);

        if(killer) // Ogni fallo causa un infortunio!
            fprintf(f,"killer\n"/*-*/);

        if(wb_game)
            fprintf(f,"workbench\nwidth=%d\nheight=%d\n"/*-*/,
                      WINDOW_WIDTH, WINDOW_HEIGHT);
        else        
            fprintf(f,"width=%d\nheight=%d\n"/*-*/,
                      wanted_width, wanted_height);
   
        if(!(detail_level&USA_RADAR))
            fprintf(f,"radar=off\n"/*-*/);
        else
        {
            fprintf(f,"radar=%d\n"/*-*/, radar_position);
        }

/* Tutta la gestione dei joystick e' un po' sconfusionata, questo perche' la
 * porta primaria e' la porta 2, mentre la secondaria la porta 1 (identificata
 * da ReadJoyPort con 0), quindi devo spesso invertire i due valori come qui...
 */

        fprintf(f,"control%d=%s\n"/*-*/, 0, controls[control[1]]);
        fprintf(f,"control%d=%s\n"/*-*/, 1, controls[control[0]]);

        fprintf(f,"time=%ld\n"/*-*/,t_l);

// relativo al campo...
        fprintf(f,"field=%d\nfield_type=%d\n"/*-*/,
                field, field_type);

// nocpu (disabilitato, solo utile in sviluppo)

        if(arcade)
        {
            fprintf(f,"arcade\n"/*-*/);

            if(situation)
            {
                fprintf(f,"situation_time=%ld\n"/*-*/,situation_time);
                fprintf(f,"situation_result=%d-%d\n"/*-*/,situation_result[0],situation_result[1]);
            }
        }

// team non serve, uso quelle predefinite.

        if(nopari)    
            fprintf(f,"nopari\n"/*-*/);

        if(training)
            fprintf(f,"training\n"/*-*/);

        if(free_longpass)
            fprintf(f,"freepass\n"/*-*/);

        fprintf(f,"strictness=%d\n", strictness);
        
        BFP(f,"crowd=%s\n"/*-*/,use_crowd);
        BFP(f,"speaker=%s\n"/*-*/,use_speaker);
        BFP(f,"autoreplay=%s\n"/*-*/,use_replay);
        BFP(f,"menumusic=%s\n"/*-*/,menu_music);
        BFP(f,"result=%s\n"/*-*/,(detail_level&USA_RISULTATO));
        BFP(f,"offside=%s\n"/*-*/,offside);

        fclose(f);
    }
    else
    {
        D(bug("File di configurazione non trovato!\n"));
    }
}

