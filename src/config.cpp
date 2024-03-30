#include "config.hpp"
#include "logging.hpp"

extern modloader::ModInfo modInfo;
Config config;

Configuration& get_config() {
    static Configuration config{modInfo};
    config.Load();
    return config;
}

void SaveConfig() {
    INFO("Saving config...");
    auto& doc = get_config().config;
    auto& allocator = doc.GetAllocator();
    doc.Clear();
    doc.SetObject();

    doc.AddMember("communitiesDiscoveryURL", config.communitiesDiscoveryURL, allocator);
    rapidjson::Value enabledCommunities;
    enabledCommunities.SetArray();
    for (auto const& community : config.enabledCommunities) {
        enabledCommunities.PushBack(community.Serialize(allocator), allocator);
    }
    doc.AddMember("enabledCommunities", enabledCommunities, allocator);

    rapidjson::Value disabledCommunities;
    disabledCommunities.SetArray();
    for (auto const& community : config.disabledCommunities) {
        disabledCommunities.PushBack(community.Serialize(allocator), allocator);
    }
    doc.AddMember("disabledCommunities", disabledCommunities, allocator);

    get_config().Write();
    INFO("Config saved!");
}

bool LoadConfig() {
    INFO("Loading config...");
    bool foundEverything = true;
    auto& doc = get_config().config;

    auto communitiesDiscoveryURLItr = doc.FindMember("communitiesDiscoveryURL");
    if (communitiesDiscoveryURLItr != doc.MemberEnd() && communitiesDiscoveryURLItr->value.IsString()) {
        config.communitiesDiscoveryURL = communitiesDiscoveryURLItr->value.Get<std::string>();
    } else {
        foundEverything = false;
    }

    auto enabledCommunitiesItr = doc.FindMember("enabledCommunities");
    if (enabledCommunitiesItr != doc.MemberEnd() && enabledCommunitiesItr->value.IsArray()) {
        config.enabledCommunities.clear();
        for (auto const& community : enabledCommunitiesItr->value.GetArray()) {
            if (!config.enabledCommunities.emplace_back().Deserialize(community)) foundEverything = false;
        }
    } else {
        foundEverything = false;
    }

    auto disabledCommunitiesItr = doc.FindMember("disabledCommunities");
    if (disabledCommunitiesItr != doc.MemberEnd() && disabledCommunitiesItr->value.IsArray()) {
        config.disabledCommunities.clear();
        for (auto const& community : disabledCommunitiesItr->value.GetArray()) {
            if (!config.disabledCommunities.emplace_back().Deserialize(community)) foundEverything = false;
        }
    } else {
        foundEverything = false;
    }

    if (foundEverything) INFO("Config loaded!");
    return foundEverything;
}

bool Community::Deserialize(rapidjson::Value const& val) {
    bool foundEverything = true;
    auto communityNameItr = val.FindMember("communityName");
    if (communityNameItr != val.MemberEnd() && communityNameItr->value.IsString()) {
        communityName = communityNameItr->value.Get<std::string>();
    } else {
        foundEverything = false;
    }
    auto communityBackgroundURLItr = val.FindMember("communityBackgroundURL");
    if (communityBackgroundURLItr != val.MemberEnd() && communityBackgroundURLItr->value.IsString()) {
        communityBackgroundURL = communityBackgroundURLItr->value.Get<std::string>();
    } else {
        foundEverything = false;
    }
    auto communityPageURLItr = val.FindMember("communityPageURL");
    if (communityPageURLItr != val.MemberEnd() && communityPageURLItr->value.IsString()) {
        communityPageURL = communityPageURLItr->value.Get<std::string>();
    } else {
        foundEverything = false;
    }
    return foundEverything;
}

rapidjson::Value Community::Serialize(rapidjson::Document::AllocatorType& allocator) const {
    rapidjson::Value val;
    val.SetObject();
    val.AddMember("communityName", communityName, allocator);
    val.AddMember("communityBackgroundURL", communityBackgroundURL, allocator);
    val.AddMember("communityPageURL", communityPageURL, allocator);
    return val;
}
