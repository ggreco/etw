#pragma once

#include "db.h"
#include <string>
#include <vector>
#include <map>
#include "gamestate.h"

struct Player {
    std::string id, name, surname, shirtNumber, teamId, role;
};

struct Team {
    std::string id, name;
    std::vector<uint8_t> players;
};

struct team_disk;
struct game;
struct team;

class Insight {
    SqliteDb db_;
    std::string game_id_, home_team_id_, scheduled_date_;
    std::map<std::string, Team> teams_;
    std::map<int32_t, GameState<11>> tracking_;
    std::map<uint8_t, Player> player_map_;
    std::string project_path_;
    std::map<int, std::string> left_team_;
    bool home_is_left_ = false;

    void get_players(int, char **, char **, std::map<std::string, Player> *players);
public:
    bool open(const std::string &project);

    Insight(const std::string &project = "") {
        if (!project.empty() && !open(project))
            throw std::string("invalid project");
    }

    void fillTeam(team_disk &, bool isLeft);

    void trackingFrame(game &game, int64_t ts);
    void positionPlayers(team &team, const PlayerState *p1, const PlayerState *p2, double elapsed);
};