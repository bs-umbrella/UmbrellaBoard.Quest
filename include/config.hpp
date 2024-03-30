#pragma once

#include <vector>
#include <string>
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "_config.h"

struct Community {
    std::string communityName;
    std::string communityBackgroundURL;
    std::string communityPageURL;

    bool Deserialize(rapidjson::Value const& value);
    rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const;
};

struct Config {
    /// @brief URL to discover new communities from
    std::string communitiesDiscoveryURL = COMMUNITIES_URL;

    /// @brief communities that are enabled and available in the board view
    std::vector<Community> enabledCommunities;
    /// @brief communities that are disabled and will not be shown in the board view
    std::vector<Community> disabledCommunities;
};

extern Config config;
void SaveConfig();
bool LoadConfig();
