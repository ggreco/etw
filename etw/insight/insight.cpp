#include "insight.h"
#include "cfile.h"
#include "rapidjson/document.h"
#include "../include/mydebug.h"
#include "../include/teamsetup.h"

void kprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

extern "C" {
void *open_insight_project(const char *projectName);
void close_insight_project(void *handle) { delete static_cast<Insight *>(handle); }

team_disk get_left_team(void *handle) { 
    const auto i = static_cast<Insight *>(handle);
    team_disk team;
    i->fillTeam(team, true);
    return team;
}

team_disk get_right_team(void *handle) { 
    const auto i = static_cast<Insight *>(handle);
    team_disk team;
    i->fillTeam(team, true);
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

    for (auto it = mapping.MemberBegin(); it != mapping.MemberEnd(); ++it) {
        player_map_[atoi(it->name.GetString())] = it->value.GetString();
    }

    D(bug("Got %ld players for match %s\n", player_map_.size(), game_id_.c_str()));

    // read the teams
    db_.query("select id, firstName, lastName, shirtNumber, teamId, role from player", this, &Insight::get_players);
 
    for (const auto &p: players_) {
        auto it = teams_.find(p.second.teamId);
        if (it == teams_.end()) {
            Team team;
            team.id = p.second.teamId;
            db_.get_fields("select name from team where id='" + p.second.teamId + "'", team.name);
            team.players.push_back(p.second.id);
            teams_[team.id] = team;
        } else
            it->second.players.push_back(p.second.id);
    }
    db_.close();

    CFile tracking(project_path_ + DEFAULT_PATH_CHAR + game_id_ + "_tr.bin");

    if (!tracking.good())
        throw std::string("Unable to open tracking file");

    for (size_t i = 0; i < 1000; ++i) {
        GameState gs;
        tracking.read(&gs, sizeof(gs));
        tracking_.emplace(gs.gameTime, gs);
    }
    tracking.close();

    auto tid = teams_.find(home_team_id_);

    if (tid == teams_.end())
        throw std::string("Unable to find team " + home_team_id_);
    D(bug("Home team is %s (%s), left is %s\n", tid->second.name.c_str(), tid->second.id.c_str(), left_team_.begin()->second.c_str()));

    D(bug("Got %ld tracking states, starting 11:\n", tracking_.size()));
    const auto &gs = tracking_.begin()->second;
    for (size_t i = 0; i < 11; ++i) {
        const PlayerState &ps = gs.homeTeam[i];
        if (ps.playerId) {
            auto it = player_map_.find(ps.playerId);
            if (it != player_map_.end()) {
                auto pit = players_.find(it->second);
                D(bug("%s) %s (%s)\n", pit->second.shirtNumber.c_str(), pit->second.name.c_str(), pit->second.role.c_str()));
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

    memset(&team, 0, sizeof(team));
    strncpy(team.name, t.name.c_str(), sizeof(team.name));
    for (const auto &pid: t.players) {
        auto pit = players_.find(pid);
        if (pit == players_.end()) continue;
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
            team.nplayers++;
        }
    }
}

void Insight::
get_players(int cols, char **vals, char **titles) {
    if (cols > 0) {
        Player p;
        p.id = vals[0];
        p.name = vals[1];
        p.surname = vals[2];
        p.shirtNumber = vals[3];
        p.teamId = vals[4];
        p.role = vals[5];
        players_[p.id] = p;
    }
}