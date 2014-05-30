#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <map>
#include <string>

#include "teamsetup.h"
#include "files.h"

#include "oogtk.h"

void ReadTeam(FILE *fh, struct team_disk *s)
{
    int i;

    s->disponibilita = fread_u32(fh);
    s->nplayers = fread_u8(fh);
    s->nkeepers = fread_u8(fh);
    s->nation = fread_u8(fh);
    s->Flags = fread_u8(fh);

    for(i = 0; i < 2; i++)
    {
        s->jerseys[i].type = fread_u8(fh);
        s->jerseys[i].color0 = fread_u8(fh);
        s->jerseys[i].color1 = fread_u8(fh);
        s->jerseys[i].color2 = fread_u8(fh);
    }

    for(i = 0; i < 3; i++)
        fread(&s->tactics[i], sizeof(char), 16, fh);

    fread(s->name, sizeof(char), 52, fh);
    fread(s->allenatore, sizeof(char), 52, fh);

    // Teams always have 3 keepers and 21 players because their size is fixed!

    for(i = 0; i < 3; i++)
    {
        fread(s->keepers[i].name, sizeof(char), 20, fh);
        fread(s->keepers[i].surname, sizeof(char), 20, fh);
        s->keepers[i].value = fread_u32(fh);
        s->keepers[i].number = fread_u8(fh);
        s->keepers[i].speed = fread_u8(fh);
        s->keepers[i].Parata = fread_u8(fh);
        s->keepers[i].Attenzione = fread_u8(fh);
        s->keepers[i].nation = fread_u8(fh);
        s->keepers[i].age = fread_u8(fh);
        s->keepers[i].injury = fread_u8(fh);
        s->keepers[i].Flags = fread_u8(fh);
    }

    for(i = 0; i < 21; i++)
    {
        fread(s->players[i].name, sizeof(char), 20, fh);
        fread(s->players[i].surname, sizeof(char), 20, fh);
        s->players[i].value = fread_u32(fh);
        s->players[i].number = fread_u8(fh);
        s->players[i].speed = fread_u8(fh);
        s->players[i].tackle = fread_u8(fh);
        s->players[i].shot = fread_u8(fh);
        s->players[i].Durata = fread_u8(fh);
        s->players[i].stamina = fread_u8(fh);
        s->players[i].quickness = fread_u8(fh);
        s->players[i].nation = fread_u8(fh);
        s->players[i].creativity = fread_u8(fh);
        s->players[i].technique = fread_u8(fh);
        s->players[i].age = fread_u8(fh);
        s->players[i].injury = fread_u8(fh);
        s->players[i].Ammonizioni = fread_u8(fh);
        s->players[i].role = fread_u8(fh);
    }
}


void WriteTeam(FILE *fh, const team_disk *s)
{
    int i;

    fwrite_u32(s->disponibilita, fh);
    fwrite_u8(s->nplayers, fh);
    fwrite_u8(s->nkeepers, fh);
    fwrite_u8(s->nation, fh);
    fwrite_u8(s->Flags, fh);

    for(i = 0; i < 2; i++)
    {
        fwrite_u8(s->jerseys[i].type, fh);
        fwrite_u8(s->jerseys[i].color0, fh);
        fwrite_u8(s->jerseys[i].color1, fh);
        fwrite_u8(s->jerseys[i].color2, fh);
    }

    for(i = 0; i < 3; i++)
        fwrite(&s->tactics[i], sizeof(char), 16, fh);

    fwrite(s->name, sizeof(char), 52, fh);
    fwrite(s->allenatore, sizeof(char), 52, fh);

    // The number of keepers and players written is always the same

    for(i = 0; i < 3; i++)
    {
        fwrite(s->keepers[i].name, sizeof(char), 20, fh);
        fwrite(s->keepers[i].surname, sizeof(char), 20, fh);
        fwrite_u32(s->keepers[i].value, fh);
        fwrite_u8(s->keepers[i].number, fh);
        fwrite_u8(s->keepers[i].speed, fh);
        fwrite_u8(s->keepers[i].Parata, fh);
        fwrite_u8(s->keepers[i].Attenzione, fh);
        fwrite_u8(s->keepers[i].nation, fh);
        fwrite_u8(s->keepers[i].age, fh);
        fwrite_u8(s->keepers[i].injury, fh);
        fwrite_u8(s->keepers[i].Flags, fh);
    }

    for(i = 0; i < 21; i++)
    {
        fwrite(s->players[i].name, sizeof(char), 20, fh);
        fwrite(s->players[i].surname, sizeof(char), 20, fh);
        fwrite_u32(s->players[i].value, fh);
        fwrite_u8(s->players[i].number, fh);
        fwrite_u8(s->players[i].speed, fh);
        fwrite_u8(s->players[i].tackle, fh);
        fwrite_u8(s->players[i].shot, fh);
        fwrite_u8(s->players[i].Durata, fh);
        fwrite_u8(s->players[i].stamina, fh);
        fwrite_u8(s->players[i].quickness, fh);
        fwrite_u8(s->players[i].nation, fh);
        fwrite_u8(s->players[i].creativity, fh);
        fwrite_u8(s->players[i].technique, fh);
        fwrite_u8(s->players[i].age, fh);
        fwrite_u8(s->players[i].injury, fh);
        fwrite_u8(s->players[i].Ammonizioni, fh);
        fwrite_u8(s->players[i].role, fh);
    }
}

std::map<std::string, team_disk> teams_;

bool SaveTeams(const char *name, const championship_disk &c)
{
    if (FILE *fh = fopen(name, "wb")) {
        fwrite(&c, 1, sizeof(championship_disk), fh);
        for (std::map<std::string, team_disk>::const_iterator it = teams_.begin(); it != teams_.end(); ++it)
            WriteTeam(fh, &it->second);
        ::fclose(fh);
    }
}

bool LoadTeams(const char *name, championship_disk &campionato)
{
    if (FILE *fh=fopen(name, "rb"/*-*/)) {
        if(fread(&campionato, 1, sizeof(championship_disk), fh)==sizeof(struct championship_disk)) {

            campionato.nteams++;

            {
                int i=0;

                while(campionato.name[i]) {
                    campionato.name[i]=toupper(campionato.name[i]);
                    i++;
                }
            }

            int i;
            char *s;

            for(i=0; i<campionato.nteams; i++)    {
                team_disk t;
                ReadTeam(fh, &t);

                s=t.name;

                while(*s) {
                    *s=toupper(*s);
                    s++;
                }

                s=t.allenatore;

                while(*s) {
                    *s=toupper(*s);
                    s++;
                }
                teams_[t.name] = t;                    
            }

            fclose(fh);

            if(i==campionato.nteams)   
                return true;
        }
    }
    else {
        fprintf(stderr, "Unable to load %s!\n", name);
    }
    return false;
}


class ETWEditor : public gtk::Application, public gtk::Builder
{
        gtk::ListStore *tms_, *gk_, *pl_;
        gtk::TreeView *tv_;
        gtk::Window *window_;
        static const char *nations_[];
        static const char *roles_[];

    public:
        ETWEditor() {
            Load("gui.xml");

            tms_ = Get<gtk::ListStore>("liststore_teams");
            gk_ = Get<gtk::ListStore>("liststore_gk");
            pl_ = Get<gtk::ListStore>("liststore_pl");
            tv_ = Get<gtk::TreeView>("treeview_teams");
            window_ = Get<gtk::Window>("window_main");
            if (!tms_ || !gk_ || !tv_ || !window_ || !pl_)
                throw std::string("Invalid GUI file.");

            Get<gtk::Button>("button_load")->OnClick(&ETWEditor::load, this);
            Get<gtk::Button>("button_save")->OnClick(&ETWEditor::save, this);
            Get<gtk::Button>("button_delete")->OnClick(&ETWEditor::delete_team, this);
            tv_->Selection().OnChanged(&ETWEditor::selection_changed, this);
            window_->OnDelete(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application*>(this));
            Get<gtk::Button>("button_quit")->OnClick(&gtk::Application::QuitLoop, dynamic_cast<gtk::Application*>(this));
            
            window_->Show();
        }

        void selection_changed() {
            gtk::TreeSelection &sel = tv_->Selection();
            gtk::RefVec v = sel.SelectedRows();

            if (v.size() != 0) {
                void *item;
                tms_->GetValue(v.begin()->Iter(), 1, item);
                team_disk *t = (team_disk *)item;
                if (t)
                    select(*t);
            }
            else {
                clear();
            }
        }
        void delete_team() {
            gtk::TreeSelection &sel = tv_->Selection();
            gtk::RefVec v = sel.SelectedRows();

            if (v.size() != 0) {
                std::string name;
                tms_->GetValue(v.begin()->Iter(), 0, name);
                tms_->Remove(v.begin()->Iter());

                teams_.erase(name);

                std::ostringstream os;
                os << "<b>Teams (" << teams_.size() << ")</b>";
            }
        }

        void load() {
            gtk::FileChooserDialog dialog("Choose an ETW team file to load...", window_);

            dialog.AddButton(GTK_STOCK_OK, gtk::ResponseOk);
            dialog.AddButton(GTK_STOCK_CANCEL, gtk::ResponseCancel);
            dialog.ShowAll();

            // run the dialog and if we click Ok then open the document
            if (dialog.Run() == gtk::ResponseOk) {
                championship_disk c;

                if (LoadTeams(dialog.Filename().c_str(), c)) {
                    tms_->Clear();
                    for (std::map<std::string, team_disk>::const_iterator it = teams_.begin(); it != teams_.end(); ++it) {
                        tms_->AddTail(0, it->second.name, 1, &(it->second), -1);
                    }
                    std::ostringstream os;
                    os << "<b>Teams (" << teams_.size() << ")</b>";
                    Get<gtk::Label>("label_teams")->Set(os.str());
                    Get<gtk::ComboBox>("combobox_type")->Active(c.type);
                    Get<gtk::Entry>("entry_champ")->Set(c.name);
                    os.str("");
                    os << (int)c.matches;
                    Get<gtk::Entry>("entry_matches")->Set(os.str());
                }
            }
        }
        void save() {
            if (Get<gtk::ComboBox>("combobox_type")->Active() == -1) {
                gtk::MessageDialog diag(window_,
                        gtk::DialogModal, gtk::MessageError, gtk::ButtonsCancel,
                    "<b>ETWEditor</b>\n\n"
                    "Select a tournament type before saving.");
                return;
            }
            if (teams_.size() > 64) {
                gtk::MessageDialog diag(window_,
                        gtk::DialogModal, gtk::MessageError, gtk::ButtonsCancel,
                    "<b>ETWEditor</b>\n\n"
                    "Your tournament contains %u teams, ETW handle at most 64 teams,\n"
                    "remove some teams before saving.", teams_.size());
                
                return;
            }

            gtk::FileChooserDialog dialog("Select a name for the file...", window_, 
                        gtk::FileChooser::ActionSave, 
                        make_vector(gtk::ButtonData(GTK_STOCK_OK, gtk::ResponseOk))
                                   (gtk::ButtonData(GTK_STOCK_CANCEL, gtk::ResponseCancel)) );
            dialog.ShowAll();

            if (dialog.Run() == gtk::ResponseOk) {
                championship_disk c;
                memset(&c, 0, sizeof(c));
                strncpy(c.name, Get<gtk::Entry>("entry_champ")->Get().c_str(), sizeof(c.name) - 1);
                c.win = 3;
                c.draw = 1;
                c.loss = 0;
                c.type = Get<gtk::ComboBox>("combobox_type")->Active();
                c.matches = ::atoi(Get<gtk::Entry>("entry_matches")->Get().c_str());
                c.nteams = teams_.size();
                SaveTeams(dialog.Filename().c_str(), c);
            }
        }

        void select(const team_disk &t) {
            Get<gtk::Label>("label_coach")->Set(t.allenatore);
            Get<gtk::Label>("label_team_name")->Set(t.name);

            gk_->Clear();
            for (size_t i = 0; i < t.nkeepers; ++i) {
                gk_->AddTail(0, t.keepers[i].surname,
                             1, t.keepers[i].name,
                             -1);
            }
            pl_->Clear();
            for (size_t i = 0; i < t.nplayers; ++i) {
                pl_->AddTail(0, t.players[i].surname,
                             1, t.players[i].name,
                             3, t.players[i].age,
                             4, nations_[t.players[i].nation],
                             5, t.players[i].number,
                             6, t.players[i].speed,
                             7, t.players[i].shot,
                             8, t.players[i].tackle,
                             -1);
            }
        }

        void clear() {
            Get<gtk::Label>("label_coach")->Set("-");
            Get<gtk::Label>("label_team_name")->Set("-");
            gk_->Clear();
            pl_->Clear();
        }
};


const char *ETWEditor::nations_[] = {"ITA", "BRA", "ARG", "ENG", "HOL", "BEL", "FRA", "ESP", "POR", "GER", "IRL", "AUT", "SWI", "TCH", "SLO", "RUS", "UKR", "SER", "CRO", "GRE", "ROM", "SWE", "NOR", "FIN", "DEN", "POL", "URU", "USA", "CAN", "BUL", "MEX", "SCO", "MAR", "CHL", "UNK", "SUD", "SAU", "NIG", "UNK", "PAR", "IRN", "TUN", "COL", "JAP", "JAM" };
const char *ETWEditor::roles_[] = {"X", "D", "M", "DM", "F", "DF", "MF", "DMF"};

int main(int argc, char *argv[]) 
{
    ETWEditor ed;

    ed.Run();

    return 0;
}

