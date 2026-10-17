#ifndef DL_TEMPLATE_GAMEDATA_H
#define DL_TEMPLATE_GAMEDATA_H
#include "Player.h"
#include <unordered_map>
struct GameData{
    Player player{};
    std::unordered_map<int, std::unordered_map<int, int>> world{};

};

#endif