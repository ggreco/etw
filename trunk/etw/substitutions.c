#include "menu.h"
#include "team.h"
#include "etw_locale.h"

extern struct Button button_substitutions[];
extern struct Button panel_substitutions[];
uint8_t game_colore_team[3]={14, 9, 6};
uint8_t game_highlight_team[3]={1, 7, 8};

#include "defines.h"

extern const char *empty;
extern int16_t swaps;
extern void add_change(team_t *t, const char *in, const char *out);
extern void SetComando(player_t *g, int8_t cmd, int8_t dopo, int8_t arg);
extern BOOL using_tactic(team_t *, const char *);
extern struct team_disk *find_disk_team(team_t *);

const char *build_name(const char *name, const char *surname)
{
    static char namebuffer[80];
    char *c=namebuffer;

    *c++='-';

    if(*name) {
        if(*surname) {
            // if name and surname are the same we skip this place, otherwise we'll get things like R.Ronaldo for Ronaldo
            if (strcmp(name, surname)) {
                *c++=toupper(*name);
                *c++='.';
            }
        }
        else while (*name) {
            *c++ =  *name;
            name++;
        }
    }

    while(*surname) {
        *c++=toupper(*surname);
        surname++;
    }

    *c=0;

    return namebuffer;
}

static void AddName(const char *name, const char *surname, int posizione)
{
    if(button_substitutions[posizione*2+1].Text)
        free(button_substitutions[posizione*2+1].Text);

    button_substitutions[posizione*2+1].Text=strdup(build_name(name, surname));
}

static void SetPlayerStatus(int posizione, char infortuni, char ammonizioni, long v)
{
    int i;

    if(panel_substitutions[posizione*3].Text!=NULL &&
        panel_substitutions[posizione*3].Text != empty)
        free(panel_substitutions[posizione*3].Text);

    if(infortuni) {
        D(bug("Mark injuried for %ld\n", posizione));

        panel_substitutions[posizione*3].Text=calloc(2, 1);

        panel_substitutions[posizione*3].Text[0]=13;
        
    }
    else if(ammonizioni == 1) {
        D(bug("Mark yellow card for %ld\n", posizione));

        panel_substitutions[posizione*3].Text=calloc(2, 1);

        panel_substitutions[posizione*3].Text[0]=5;
    }
    else if (ammonizioni == 2) {
        D(bug("Mark red card for %ld\n", posizione));

        panel_substitutions[posizione*3].Text=calloc(2, 1);
        panel_substitutions[posizione*3].Text[0]=7;
    }
    else
        panel_substitutions[posizione*3].Text=(char*)empty;

    if(panel_substitutions[posizione*3+2].Text==NULL ||
        panel_substitutions[posizione*3+2].Text == empty)
        panel_substitutions[posizione*3+2].Text=malloc(6);

    panel_substitutions[posizione*3+2].Text[5]=13;

// Deve dipendere dal valore!

    if(v<1)
    {
        panel_substitutions[posizione*3+2].Text[0]=' ';
        panel_substitutions[posizione*3+2].Text[1]=0;
        return;
    }
    
    v--;

    if(v>9)
        v=9;

//    D(bug("value: %ld -> %ld\n", value, v));

    for(i=0; i<10; i+=2)
    {
        if(i<=v)
            panel_substitutions[posizione*3+2].Text[i/2]= ( (i<v) ? i/2 : (i/2 + 8) );
        else
            panel_substitutions[posizione*3+2].Text[i/2]=13;
    }
}

static void AddPlayer(player_t *g, int posizione)
{
    extern player_t *has_player_injuried();
    char buffer[8];

    AddName(g->name, g->surname, posizione);

    *buffer=0;
    
    if(g->Posizioni&P_DIFESA)
        strcat(buffer, msg_3);
    if(g->Posizioni&P_CENTRO)
        strcat(buffer, msg_4);
    if(g->Posizioni&P_ATTACCO)
        strcat(buffer, msg_5);

    if(panel_substitutions[posizione*3+1].Text&&panel_substitutions[posizione*3+1].Text!=empty)
        free(panel_substitutions[posizione*3+1].Text);

    button_substitutions[posizione*2].Text=(char*)numbers[g->number];

    panel_substitutions[posizione*3+1].Text=strdup(buffer);

    SetPlayerStatus(posizione, g->Ammonito & 4, g->Ammonito & 3,
        (((g->Tiro+g->tackle+g->speed*2+g->technique+g->creativity-2*6+3)*10)/7)/6 );
}

static void AddRiserva(struct player_disk *g, int posizione)
{
    char buffer[8];

    if(panel_substitutions[posizione*3+1].Text&&panel_substitutions[posizione*3+1].Text!=empty)
        free(panel_substitutions[posizione*3+1].Text);

    if (g->number == 255) {        
        if(button_substitutions[posizione*2+1].Text)
            free(button_substitutions[posizione*2+1].Text);

        if(panel_substitutions[posizione*3].Text!=NULL &&
            panel_substitutions[posizione*3].Text != empty)
            free(panel_substitutions[posizione*3].Text);
        if(panel_substitutions[posizione*3 + 2].Text!=NULL &&
            panel_substitutions[posizione*3 + 2].Text != empty)
            free(panel_substitutions[posizione*3 + 2].Text);

        button_substitutions[posizione*2+1].Text=NULL;
        button_substitutions[posizione*2].Text=NULL;
        panel_substitutions[posizione*3+1].Text=panel_substitutions[posizione*3].Text=panel_substitutions[posizione*3+2].Text=NULL;
        return;
    }
    button_substitutions[posizione * 2].Text=(char*)numbers[g->number];

    AddName(g->name, g->surname, posizione);

    *buffer=0;
    
    if(g->Posizioni&P_DIFESA)
        strcat(buffer, msg_3);
    if(g->Posizioni&P_CENTRO)
        strcat(buffer, msg_4);
    if(g->Posizioni&P_ATTACCO)
        strcat(buffer, msg_5);

    panel_substitutions[posizione*3+1].Text=strdup(buffer);

    SetPlayerStatus(posizione, FALSE, FALSE,
        (((g->Tiro+g->tackle+g->speed*2+g->technique+g->creativity-2*6+3)*10)/7)/6 );
}

extern struct GfxMenu *actual_menu;
extern BOOL change_tactic(team_t*, const char*);
extern BOOL CheckBall(player_t*);

static team_t *sub_team = NULL;
static struct team_disk *sd = NULL;

static player_t *has_player(const char *name)
{
    int i;

    for (i = 0; i < 10; ++i) 
        if (!strcmp(name, build_name(sub_team->players[i].name, sub_team->players[i].surname)))
            return &sub_team->players[i];
    
    return NULL;
}

static struct player_disk *has_substitution(const char *name)
{
    int i;

    for (i = 10; i < 16; ++i) {
        if (sd->players[i].number == 255)
            continue;

        if (!strcmp(name, build_name(sd->players[i].name, sd->players[i].surname)))
            return &sd->players[i];
    }

    return NULL;
}
static BOOL perform_substitutions()
{
    int i;
    int subst = 0;
    struct Button *bt = actual_menu->Button;
    // we iterate the layout of the formation that the player did.
    // we have to identify position changes from real changes and perform them.
    // if there are more real changes that the team can pop up an error form
    // and return FALSE

    // to begin we count substitutions to see if we can perform the operation
    // we have substituted the goalkeeper?
    if (strcmp(build_name(sub_team->keepers.name, sub_team->keepers.surname), bt[1].Text))
        subst++;

    for (i = 1; i < 11; ++i) 
        if (!has_player( bt[i * 2 + 1].Text))
            subst++;
    
    if ((subst + sub_team->Sostituzioni) > 3) {
        request(msg_280);
        return FALSE;
    }
    D(bug("Preparing to perform %d substitutions\n", subst));

    // change the goalkeeper
    if (strcmp(build_name(sub_team->keepers.name, sub_team->keepers.surname), bt[1].Text)) {
        extern void MakeName(player_t *, struct player_disk *);
        D(bug("Changing goalkeeper %s with %s\n", sub_team->keepers.name, bt[1].Text));
        add_change(sub_team, build_name(sub_team->keepers.name, sub_team->keepers.surname), bt[1].Text);
        free(sub_team->keepers.name);
        sub_team->keepers.number = sd->keepers[1].number;
        sub_team->keepers.Parata = sd->keepers[1].Parata;
        sub_team->keepers.Attenzione = sd->keepers[1].Attenzione;
        sub_team->keepers.velocita = sd->keepers[1].speed;
        MakeName((player_t *)&sub_team->keepers, (struct player_disk *)&sd->keepers[1]);
        sub_team->keepers.SpecialData = ENTRA_CAMPO;
        sub_team->keepers.TimePress = 100;

        // we can change the goalkeeper only once
        sd->nkeepers = 1;
    }

    // first we perform all the role changes
    for (i = 1; i < 11; ++i) {
        char actual_player_name[80], *new_player_name = bt[i * 2 + 1].Text;
        player_t *new_plr = has_player(new_player_name);

        strcpy(actual_player_name, build_name(sub_team->players[i - 1].name, 
                                              sub_team->players[i - 1].surname));

        // if the new player exists and in that position has a different name of the one there was before
        if (new_plr && strcmp(new_player_name, actual_player_name)) {
                player_t *old_plr;
                // if the player that was in that position is still on the 11...
                if ((old_plr = has_player(actual_player_name))) {
                    int temp_gnum;
                    player_t temp;
                    D(bug("Swapping positions of %s and %s\n", new_player_name, actual_player_name));
                    // let's swap them
                    temp_gnum = new_plr->GNum;
                    temp = *new_plr;
                    *new_plr = *old_plr;
                    *old_plr = temp;
                    new_plr->GNum = i - 1;
                    old_plr->GNum = temp_gnum;
                
            }
        }
    }

    for (i = 1; i < 11; ++i) {
        char actual_player_name[80], *new_player_name = bt[i * 2 + 1].Text;
        struct player_disk *new_plr;
        player_t *old_plr;

        // was already on pitch, it's not in our interest
        if (has_player(new_player_name))
            continue;

        new_plr = has_substitution(new_player_name);

        if (!new_plr) {
            D(bug("ERROR!!!! Player %s is not available in the changes, ignoring transfer!\n", new_player_name));
            continue;
        }
        strcpy(actual_player_name, build_name(sub_team->players[i - 1].name, 
                                              sub_team->players[i - 1].surname));

        if ((old_plr = has_player(actual_player_name))) {
            int best;
            extern void ChangePlayer(struct player_disk *src, player_t *dest);
             D(bug("New player in: %s out: %s\n", new_player_name, actual_player_name));
            add_change(sub_team, actual_player_name, new_player_name);
            for (best = 1; best < 7; ++best)
                if (new_plr == &sd->players[best + 9])
                    break;

            // we do the "exit pitch" animation only if we don't have the ball
            // for instance because it's a kick off or a freekick/corner
            if (!old_plr->Special && !CheckBall(old_plr)) {
                old_plr->Comando=ESCI_CAMPO;
                old_plr->CA[0]=best;
                old_plr->CA[1] = 0;
            }
            else {
                D(bug("Force in place exchange for players!\n"));
                ChangePlayer(new_plr, old_plr);
                // now it's new!
                SetComando(old_plr, ENTRA_CAMPO, 0, 100);
            }
        }
        else {
            D(bug("ERROR!!!! Player %s to be substituted with %s is not on pitch, ignoring transfer!\n", actual_player_name, new_player_name));
            continue;
        }
    }

    for (i = 0; i < 6; i++) {
        struct Button *t = &bt[34 + i];
        if (t->Color == COLOR_TATTICA_SELEZIONATA && !using_tactic(sub_team, t->Text))
            change_tactic(sub_team, t->Text);
    }

    sub_team->Sostituzioni += subst;
    // we return TRUE if the operation can, and has been done
    return TRUE;
}


static BOOL not_red_card_substitution(int p1, int p2)
{
    struct Button *b = &actual_menu->Pannello[p1 * 3],
                  *b2 =  &actual_menu->Pannello[p2 * 3];

    // if both players are on pitch I can move them
    if (p1 > 0 && p1 < 11 && p2 > 0 && p2 < 11)
        return TRUE;

    // if both players do not have any "comment" I can move them
    if ((b->Text == NULL || b->Text == empty) && (b2->Text == NULL | b2->Text == empty))
        return TRUE;

    // if both players are not subject to a red card (7), I can move them
    return (b->Text[0] != 7 && b2->Text[0] != 7);
}


static void redraw_tactic(struct Button *b)
{
    extern void blit_scaled_logo();
    int i;
    const char *nums[PLAYERS];

    for (i = 0; i < PLAYERS; ++i) { 
        if (actual_menu->Pannello[(i + 1) * 3].Text &&
            actual_menu->Pannello[(i + 1) * 3].Text[0] == 7)
            nums[i] = NULL;
        else
            nums[i] = actual_menu->Button[(i + 1) * 2].Text;     
    }

    bltchunkybitmap(back, actual_menu->X, actual_menu->Y, main_bitmap,
            actual_menu->X, actual_menu->Y, 
            FixedScaledX(108), FixedScaledY(108), bitmap_width, bitmap_width);
    blit_scaled_logo();

    DisplayTactic(b ? b->Text : sub_team->tactic->Name, actual_menu->X, actual_menu->Y, nums, sub_team->Joystick);

    for (i = 0; i < 6; i++) {
        struct Button *t = &actual_menu->Button[34 + i];
        if (b == NULL) {
            RedrawButton(t, t->Color);
            continue;
        }

        if (t == b)
            continue;
        t->Color = COLOR_TATTICA_NON_SELEZIONATA;
        RedrawButton(t, COLOR_TATTICA_NON_SELEZIONATA);
    }

    if (b) {
        b->Color = COLOR_TATTICA_SELEZIONATA;
        RedrawButton(b, COLOR_TATTICA_SELEZIONATA);
    }
}

BOOL TeamSubstitutions(int16_t button)
{
    struct Button *b;
    static int sel1 = -1;
    D(bug("Clicked button %d\n", button));

    if (button < 0)
        return TRUE;

    b = &actual_menu->Button[button];

    if (button == actual_menu->NumeroBottoni - 2) {
        // we go back to previous menu if the operation can be done (and has been done) only
        return !perform_substitutions();
    }
    else if (button == actual_menu->NumeroBottoni - 1) {
        return FALSE;
    }
    else if (button < 34) { 
        struct Button *b2;
        int pos, selected = button / 2;

        b2 = &actual_menu->Button[selected * 2 + 1];

        if (sel1 >= 0) {

            if (sel1 == selected)
                RedrawButton(b2, b2->Color);
            else {
                struct Button *b3 = &actual_menu->Button[sel1 * 2 + 1];
                int i;

                pos = selected;

                // let's exchange the goalkeeper or a pitch player, but not in between them
                if ( ((((pos == 0 && sel1 == 11) || (sel1 == 0 && pos == 11)) && sd->nkeepers > 1) || 
                        (pos != 0 && pos != 11 && sel1 != 0 && sel1 != 11)) && 
                        not_red_card_substitution(pos, sel1)
                   ) {                             
                    char *temp;
                   
                    // we swap rows

                    for (i = 0; i > -2; i--) {
                        temp = (b2 + i)->Text;
                        (b2 + i)->Text = (b3 + i)->Text;
                        (b3 + i)->Text = temp;
                        RedrawButton((b2 + i), (b2 + i)->Color);
                        RedrawButton((b3 + i), (b3 + i)->Color);
                    }

                    for (i = 0; i < 3; i++) {
                        temp = actual_menu->Pannello[i + pos * 3].Text;
                        actual_menu->Pannello[i + pos * 3].Text = actual_menu->Pannello[i + sel1 * 3].Text;
                        actual_menu->Pannello[i + sel1 * 3].Text = temp;
                        RedrawButton(&actual_menu->Pannello[sel1 * 3 + i], actual_menu->Pannello[sel1 * 3 + i].Color);
                        RedrawButton(&actual_menu->Pannello[pos * 3 + i], actual_menu->Pannello[pos * 3 + i].Color);
                    }
                    redraw_tactic(NULL);
                }
                else 
                    RedrawButton(b3, b3->Color);                   
            }
            sel1 = -1;
        }
        else {
            sel1 = selected;
            RedrawButton(b2, b2->Highlight);
        }
        ScreenSwap();
    }
    else if (button < 40) {
        if (b->Color != COLOR_TATTICA_SELEZIONATA) {
            redraw_tactic(b);
            DrawBox(button);
            ScreenSwap();
        }
    }
    return TRUE;
}
void SetTeamSubstitutions(team_t *s)
{
    int i;
    int nriserve = min(5, s->NumeroRiserve);
    sd = find_disk_team(s);

    sub_team = s;

    for (i=0; i<11; i++) {
        button_substitutions[i*2+1].Color=
            button_substitutions[i*2].Color=panel_substitutions[i*3].Color=
            panel_substitutions[i*3 + 1].Color= game_colore_team[s->Joystick+1];

        button_substitutions[i*2+1].Highlight=
            button_substitutions[i*2].Highlight=panel_substitutions[i*3].Highlight=
            panel_substitutions[i*3 + 1].Highlight= game_highlight_team[s->Joystick+1];
    }

    for (i=11; i<17; i++) {
        button_substitutions[i*2+1].Color=
            button_substitutions[i*2].Color=panel_substitutions[i*3].Color=
            panel_substitutions[i*3 + 1].Color= game_highlight_team[s->Joystick+1];

        button_substitutions[i*2+1].Highlight=
            button_substitutions[i*2].Highlight=panel_substitutions[i*3].Highlight=
            panel_substitutions[i*3 + 1].Highlight= game_colore_team[s->Joystick+1];
    }

    for (i=0; i < 10; i++) 
        AddPlayer(&s->players[i], i + 1);

    for (i = 0; i < nriserve; ++i)
        AddRiserva(&sd->players[10 + i], 12 + i);
  
    if (nriserve < 5) {
        for (i = 12 + nriserve ; i<17; i++) {
            if(button_substitutions[i*2+1].Text)
                free(button_substitutions[i*2+1].Text);

            button_substitutions[i*2+1].Text=NULL;
            button_substitutions[i*2].Text=NULL;

            if (panel_substitutions[i*3].Text && panel_substitutions[i*3].Text != empty)
                free(panel_substitutions[i*3].Text);

            if (panel_substitutions[i*3+1].Text && panel_substitutions[i*3+1].Text != empty)
                free(panel_substitutions[i*3+1].Text);

            if (panel_substitutions[i*3+2].Text && panel_substitutions[i*3+2].Text != empty)
                free(panel_substitutions[i*3+2].Text);

            panel_substitutions[i*3+1].Text=panel_substitutions[i*3].Text=panel_substitutions[i*3+2].Text=NULL;
        }
    }
    

    // set the selected tactic
    for (i=0; i<6; i++) {
        if (!stricmp(button_substitutions[34+i].Text, s->tactic->Name))
            button_substitutions[34+i].Color=COLOR_TATTICA_SELEZIONATA;
        else 
            button_substitutions[34+i].Color=COLOR_TATTICA_NON_SELEZIONATA;
    }

    AddName(s->keepers.name, s->keepers.surname, 0);
    SetPlayerStatus(0, 0, 0, (((s->keepers.Parata*2+s->keepers.Attenzione-2*3+2)*10)/7)/3);

    if (sd->nkeepers < 2) {
        panel_substitutions[11*3+2].Text=panel_substitutions[11*3+1].Text=panel_substitutions[11*3].Text=NULL;
        button_substitutions[11*2].Text=button_substitutions[11*2+1].Text=NULL;
    }
    else {
        AddName(sd->keepers[1].name, sd->keepers[1].surname, 11);
        pannelli[11*3+1].Text= msg_141;
        SetPlayerStatus(11, 0, 0,
                (sd->keepers[1].Parata*2+sd->keepers[1].Attenzione+2)/3);
    }

    D(bug("SetTeamSubstitutions for %s ok\n", s->name));
}

