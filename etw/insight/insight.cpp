#include "insight.h"
#include "cfile.h"
#include "rapidjson/document.h"
#include "mydebug.h"
#include "teamsetup.h"
#include "defines.h"

void kprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

#include <clocale>
#include <cwchar>
#include <iostream>

// partial unicode conversion table, will need more entries or an external library
std::map<unsigned char, char> unicodePageConversion = {
    { 188, 'u' },
    { 135, 'C' },
    { 182, 'o' },
    { 169, 'e' },
    { 195, 'i' }
};

std::map<std::string, jersey> jerseyMap = {
    {"13", {JERSEY_STRIPES, P_BLU2, P_BLU1, P_BIANCO }}, // leicester (away)
    {"21", {JERSEY_TWO_COLORS, P_ROSSO2, P_BIANCO, P_BLU2 } } // west ham (home)
};

std::string remove_unicode(const char *src) {
    std::string result;
    size_t len = strlen(src);

    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = src[i];
        if (c < 128)
            result.push_back(c);
        else if (c == 195) {
            ++i; 
            auto res = unicodePageConversion.find(src[i]);
            if (res != unicodePageConversion.end())
                result.push_back(res->second);
            else
                std::cout << "Unknown unicode char (" << (int)c << ") from " << src << std::endl;
        }    
        else
            std::cout << "Strip wide char (" << (int)c << ") from " << src << std::endl;
    }

    return result;
}

extern "C" {
#include "eat.h"

void *open_insight_project(const char *projectName);
void close_insight_project(void *handle) { delete static_cast<Insight *>(handle); }
void tracking_frame(void *handle, game_t *game, int64_t timestamp) {
    static_cast<Insight *>(handle)->trackingFrame(*game, timestamp);
}
team_disk get_left_team(void *handle) { 
    const auto i = static_cast<Insight *>(handle);
    team_disk team;
    i->fillTeam(team, true);
    return team;
}

team_disk get_right_team(void *handle) { 
    const auto i = static_cast<Insight *>(handle);
    team_disk team;
    i->fillTeam(team, false);
    return team;
}

}

void *open_insight_project(const char *projectName)
{
    try {
        return new Insight(projectName);
    } catch (std::string &err) { 
        D(bug("Unable to open insight project '%s': %s", projectName, err.c_str()));
        return nullptr;
    } catch (...) {
        D(bug("Unable to open insight project '%s': unknown error"));
        return nullptr;
    }
}

bool Insight::
open(const std::string &project)
{
    if (!db_.open(project + DEFAULT_PATH_CHAR "Db.sqlite"))
        return false;

    project_path_ = project;

    // get the first game in the db
    std::string periods_data;
    db_.get_fields("select id, homeTeamId, periods, scheduledDate from game", game_id_, home_team_id_, periods_data, scheduled_date_);
    if (game_id_.empty())
        throw std::string("No game available");

    // parse the periods json
    rapidjson::Document periods;
    periods.Parse(periods_data.c_str());

    if (!periods.IsArray())
        throw std::string("Unable to parse periods");

    for (const auto &per: periods.GetArray()) {
        const auto &p = per.GetObject();
        left_team_[p["id"].GetInt()] = p["leftTeamId"].GetString();
    }

    // read the json index for player mapping
    std::string contents;
    if (!CFile(project + DEFAULT_PATH_CHAR + game_id_ + "_tr.bin.idx").read(contents))
        throw std::string("Unable to read index file for " + game_id_);

    rapidjson::Document idx;

    idx.Parse(contents.c_str());

    if (!idx.IsObject() || !idx.HasMember("mapping"))
        throw std::string("Invalid index file");

    auto mapping = idx["mapping"].GetObject();

    std::map<std::string, uint8_t> player_map;

    for (auto it = mapping.MemberBegin(); it != mapping.MemberEnd(); ++it) {
        player_map[std::string(it->value.GetString())] = atoi(it->name.GetString());
    }

    D(bug("Got %ld players for match %s\n", player_map.size(), game_id_.c_str()));

    std::map<std::string, Player> players;
    // read the teams
    db_.query("select id, firstName, lastName, shirtNumber, teamId, role from player", this, &Insight::get_players, &players);
 
    for (const auto &p: players) {
        auto it = teams_.find(p.second.teamId);
        auto pit = player_map.find(p.first);
        if (pit != player_map.end())
            player_map_[pit->second] = p.second;
        else {
            D(bug("Ignoring player with id %s not in tracking", p.first.c_str()));
            continue;
        }

        if (it == teams_.end()) {
            Team team;
            team.id = p.second.teamId;
            std::string teamName;
            db_.get_fields("select name from team where id='" + p.second.teamId + "'", teamName);
            team.name = remove_unicode(teamName.c_str());
            team.players.push_back(pit->second);
            teams_[team.id] = team;
        } else
            it->second.players.push_back(pit->second);
    }
    db_.close();

    CFile tracking(project_path_ + DEFAULT_PATH_CHAR + game_id_ + "_tr.bin");

    if (!tracking.good())
        throw std::string("Unable to open tracking file");

    for (size_t i = 0; i < 2000; ++i) {
        GameState gs;
        tracking.read(&gs, sizeof(gs));
        // D(bug("%ld) TS:%d P:%d B:%d,%d\n", i, gs.gameTime, gs.period, gs.ballPosition[0], gs.ballPosition[1]));
        gs.gameTime = SDL_SwapBE32(gs.gameTime);
        for (size_t j = 0; j < 4; ++j)
            gs.ballPosition[j] = SDL_SwapBE16(gs.ballPosition[j]);
        gs.worldTime = SDL_SwapBE64(gs.worldTime);     
        gs.ballPosition[1] = 25600 - gs.ballPosition[1];
        for (size_t j = 0; j < 11; ++j) {
            gs.homeTeam[j].x = SDL_SwapBE16(gs.homeTeam[j].x);
            gs.homeTeam[j].y = 25600 - SDL_SwapBE16(gs.homeTeam[j].y);
            gs.homeTeam[j].speed = SDL_SwapBE16(gs.homeTeam[j].speed);
            gs.awayTeam[j].x = SDL_SwapBE16(gs.awayTeam[j].x);
            gs.awayTeam[j].y = 25600 - SDL_SwapBE16(gs.awayTeam[j].y);
            gs.awayTeam[j].speed = SDL_SwapBE16(gs.awayTeam[j].speed);
        }

        tracking_.emplace(gs.gameTime, gs);
    }
    tracking.close();

    auto tid = teams_.find(home_team_id_);

    if (tid == teams_.end())
        throw std::string("Unable to find team " + home_team_id_);
    D(bug("Home team is %s (%s), left is %s\n", tid->second.name.c_str(), tid->second.id.c_str(), left_team_.begin()->second.c_str()));
    
    home_is_left_ = home_team_id_ == left_team_.begin()->second;

    D(bug("Got %ld tracking states, starting 11:\n", tracking_.size()));
    const auto &gs = tracking_.begin()->second;
    for (size_t i = 0; i < 11; ++i) {
        const PlayerState &ps = gs.homeTeam[i];
        if (ps.playerId) {
            auto pit = player_map_.find(ps.playerId);
            if (pit != player_map_.end()) {
                D(bug("%s) %s %s (%s)\n", pit->second.shirtNumber.c_str(), pit->second.name.c_str(), pit->second.surname.c_str(), pit->second.role.c_str()));
            }
        }
    }
    for (size_t i = 0; i < 11; ++i) {
        const PlayerState &ps = gs.awayTeam[i];
        if (ps.playerId) {
            auto pit = player_map_.find(ps.playerId);
            if (pit != player_map_.end()) {
                D(bug("%s) %s %s (%s)\n", pit->second.shirtNumber.c_str(), pit->second.name.c_str(), pit->second.surname.c_str(), pit->second.role.c_str()));
            }
        }
    }
    return true;
}

void Insight::
fillTeam(team_disk &team, bool isLeft)
{
    std::string teamId;

    if (isLeft)
        teamId = left_team_.begin()->second;
    else {
        for (const auto &t: teams_) {
            if (t.first != left_team_.begin()->second) {
                teamId = t.first;
                break;
            }
        }
    }
    auto tit = teams_.find(teamId);

    if (tit == teams_.end()) {
        D(bug("Unable to find team: ", isLeft ? "LEFT" : "RIGHT"));
        return;
    }
    const auto &t = tit->second;

    D(bug("Filling %s team: %s\n", isLeft ? "left" : "right", t.name.c_str()));

    memset(&team, 0, sizeof(team));
    strncpy(team.name, t.name.c_str(), sizeof(team.name));
    
    auto jit = jerseyMap.find(t.id);
    if (jit != jerseyMap.end())
        team.jerseys[0] = jit->second;
    else {
        D(bug("Jersey not found for team %s (%s)\n", t.name.c_str(), t.id.c_str()));
    }
    for (const auto &pid: t.players) {
        auto pit = player_map_.find(pid);
        if (pit == player_map_.end()) continue;
        const auto &p = pit->second;
        if (p.role == "Goalkeeper") {
            auto &gk = team.keepers[team.nkeepers];
            strncpy(gk.name, p.name.c_str(), sizeof(gk.name));
            strncpy(gk.surname, p.surname.c_str(), sizeof(gk.surname));
            gk.number = atoi(p.shirtNumber.c_str());
            team.nkeepers++;
        } else {
            auto &gk = team.players[team.nplayers];
            strncpy(gk.name, p.name.c_str(), sizeof(gk.name));
            strncpy(gk.surname, p.surname.c_str(), sizeof(gk.surname));
            gk.number = atoi(p.shirtNumber.c_str());
            gk.speed = 5;
            team.nplayers++;
        }
    }
}

const double fieldHeight = CORNER_Y_S - CORNER_Y_N;
const double fieldWidth = CORNER_X_SE - CORNER_X_SO;

inline double interpolatedValue(int16_t v1, int16_t v2, double elapsed) {
    double v1d = (double)v1 / 256.0, v2d = (double)v2 / 256.0;
    return v1d + (v2d - v1d) * elapsed;
}
inline int16_t remapXcoordinate(int16_t x1, int16_t x2, double elapsed) { 
    return (int16_t)(CORNER_X_SO + interpolatedValue(x1, x2, elapsed) / 100 * fieldWidth);
}

inline int16_t remapYcoordinate(int16_t y1, int16_t y2, double elapsed) { 
    return (int16_t)(CORNER_Y_N + interpolatedValue(y1, y2, elapsed) / 100 * fieldHeight);
}

void Insight::
trackingFrame(game_t &game, int64_t timestamp) {
    auto it = tracking_.upper_bound(timestamp);
    if (it == tracking_.end()) {
        D(bug("Unable to find a tracking frame for %ld", timestamp));
        return;
    }
    const auto &gs2 = it->second;
    auto it2 = it != tracking_.begin() ? --it : it;
    const auto &gs1 = it2->second;

    double elapsed = gs1.gameTime == gs2.gameTime ? 0 : (double)(timestamp - gs1.gameTime) / (double)(gs2.gameTime - gs1.gameTime);
    if (elapsed > 1)
        elapsed = 1;
    // D(bug("Working with gamestate with ts %d to %d for %ld, elapsed %lf quota %d\n", gs1.gameTime, gs2.gameTime, timestamp, elapsed, gs1.ballPosition[2]));

    game.ball.world_x = remapXcoordinate(gs1.ballPosition[0], gs2.ballPosition[0], elapsed);
    game.ball.world_y = remapYcoordinate(gs1.ballPosition[1], gs2.ballPosition[1], elapsed);
    game.ball.quota = min(gs1.ballPosition[2] / 100, MAX_QUOTA - 1);
    if (home_is_left_) {
        positionPlayers(*game.team[0], gs1.homeTeam, gs2.homeTeam, elapsed);
        positionPlayers(*game.team[1], gs1.awayTeam, gs2.awayTeam, elapsed);
    } else {
        positionPlayers(*game.team[0], gs1.awayTeam, gs2.awayTeam, elapsed);
        positionPlayers(*game.team[1], gs1.homeTeam, gs2.homeTeam, elapsed);
    }
}

void Insight::
positionPlayers(team_t &team, const PlayerState *players1, const PlayerState *players2, double elapsed)
{
    for (size_t i = 0; i < 11; ++i) {
        const auto &ps1 = players1[i];
        if (!ps1.playerId) continue;
        size_t k;
        for (k = 0; k < 11; ++k) {
            if (players2[k].playerId == ps1.playerId)
                break;
        }
        if (k == 11) continue;
        const auto &ps2 = players2[k];

        auto x = G2P_X(remapXcoordinate(ps1.x, ps2.x, elapsed)),
             y = G2P_Y(remapYcoordinate(ps1.y, ps2.y, elapsed));
        int64_t delta = (ps2.x - ps1.x) * (ps2.x - ps1.x) + (ps2.y - ps1.y) * (ps2.y - ps1.y);

        if (team.keepers.number == ps1.shirtNumber) {
            team.keepers.world_x = x;
            team.keepers.world_y = y;
            team.keepers.dir =  FindDirection(ps1.x, ps1.y, ps2.x, ps2.y);
            if (delta > 10) {
                if (!team.keepers.ActualSpeed) {
                    DoAnim((&team.keepers), PORTIERE_CORSA);
                    team.keepers.ActualSpeed = 1;
                }
            } else if (team.keepers.ActualSpeed) {
                team.keepers.ActualSpeed = 0;
                DoAnim((&team.keepers), PORTIERE_FERMO);
            }
        } else 
            for (size_t j = 0; j < 10; ++j) {
                auto &pl = team.players[j];
                if (pl.number == ps1.shirtNumber) {
                    pl.world_x = x;
                    pl.world_y = y;

                    //D(bug("Delta movement of %d is %ld", pl.number, delta));
                    pl.dir =  FindDirection(ps1.x, ps1.y, ps2.x, ps2.y);

                    if (delta > 50) {
                        if (!pl.ActualSpeed) {
                            int a = GIOCATORE_CORSA_VELOCE;
                            if (delta < 100)
                                a = GIOCATORE_CORSA_LENTA;
                            else if (delta < 200)
                                a = GIOCATORE_CORSA_MEDIA;

                            DoAnim((&pl), a);
                            pl.ActualSpeed = 1;
                        }
                    } else if (pl.ActualSpeed) {
                        DoAnim((&pl),GIOCATORE_RESPIRA);
                        pl.ActualSpeed=0;
                    }
                    break;
                }
            }
    }
}

void Insight::
get_players(int cols, char **vals, char **titles, std::map<std::string, Player> *players) {
    if (cols > 0) {
        Player p;
        p.id = vals[0];
        p.name = remove_unicode(vals[1]);
        p.surname = remove_unicode(vals[2]);
        p.shirtNumber = vals[3];
        p.teamId = vals[4];
        p.role = vals[5];
        players->emplace(p.id, p);
    }
}