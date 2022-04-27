#include "insight.h"
#include "cfile.h"
#include "rapidjson/document.h"
#include "../include/mydebug.h"

void kprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

extern "C" {
void *open_insight_project(const char *projectName);
void close_insight_project(void *);
}

void close_insight_project(void *project)
{
    delete static_cast<Insight *>(project);
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
    db_.get_fields("select id, homeTeamId, periods from game", game_id_, home_team_id_, periods_data);
    if (game_id_.empty())
        throw std::string("No game available");
    // parse the periods json
    rapidjson::Document periods;
    periods.Parse(periods_data.c_str());

    if (!periods.IsArray())
        throw std::string("Unable to parse periods");

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
    db_.query("select id, knownName, shirtNumber, teamId, role from player", this, &Insight::get_players);
 
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
    D(bug("Home team is %s\n", tid->second.name.c_str()));

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
get_players(int cols, char **vals, char **titles) {
    if (cols > 0) {
        Player p;
        p.id = vals[0];
        p.name = vals[1];
        p.shirtNumber = vals[2];
        p.teamId = vals[3];
        p.role = vals[4];
        players_[p.id] = p;
    }
}