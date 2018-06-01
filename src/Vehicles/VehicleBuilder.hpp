#pragma once
#include "common.hpp"

class IModule;
class Player;

class VehicleBuilder
{
    std::string m_configName;
public:
    VehicleBuilder(const std::string& configName) : m_configName(configName){}

    void build(Player& player);

};
