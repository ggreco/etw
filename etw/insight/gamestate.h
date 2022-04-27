#pragma once
#pragma pack(push, 1)
struct PlayerState {
    int16_t x, y, speed; // fixed float, use / 256 to get to the original floating value
    int8_t shirtNumber; // -127 is undefined
    uint8_t playerId; // 0 is undefined
};

template <size_t N=11> 
struct GameState {
    uint64_t worldTime;
    int16_t ballPosition[4]; // x, y, z, speed
    int8_t period;
    uint8_t flags; // 1 left possession, 2 right possession, 4 inGame
    float gameTime;
    PlayerState homeTeam[N];
    PlayerState awayTeam[N];
};

#pragma pack(pop)