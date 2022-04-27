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
    std::vector<std::string> players;
};

struct team_disk;

class Insight {
    SqliteDb db_;
    std::string game_id_, home_team_id_, scheduled_date_;
    std::map<std::string, Team> teams_;
    std::map<std::string, Player> players_;
    std::map<float, GameState<11>> tracking_;
    std::map<uint8_t, std::string> player_map_;
    std::string project_path_;
    std::map<int, std::string> left_team_;

    void get_players(int, char **, char **);
public:
    bool open(const std::string &project);

    Insight(const std::string &project = "") {
        if (!project.empty() && !open(project))
            throw std::string("invalid project");
    }

    void fillTeam(team_disk &, bool isLeft);
};