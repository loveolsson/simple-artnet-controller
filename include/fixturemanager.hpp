#pragma once
#include "fixture.hpp"
#include "jsoncache.hpp"
#include "settings.hpp"

#include <concurrentqueue.h>
#include <nlohmann/json_fwd.hpp>

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>

class FixtureManager
{
    using ArrStr = std::array<char, 64>;

    struct Instruction {
        bool createFixture;
        bool deleteFixture;
        bool setName;
        bool setUniverse;
        bool setChannel;
        bool setLocation;
        FixtureId id;
        int x;
        int y;
        ArrStr str;
    };

    std::map<std::string, nlohmann::json> fixtureDescs;
    std::map<FixtureId, Fixture> fixtures;

    FixtureId FindFreeId();
    void LoadFixtureDescs();
    void LoadFixtureConf(Settings &);

    moodycamel::ConcurrentQueue<Instruction> instructionQueue;

    FixtureId CreateFixtureInternal(const std::string &type);

public:
    JSONCache jsonFixtureDescs;
    JSONCache jsonConfigCache;

    void Init(Settings &);
    void StoreSettings(Settings &);
    Fixture *GetFixture(FixtureId, bool onlyActive = true);
    Fixture *GetFixture(const std::string &name, bool onlyActive = true);

    void CreateFixture(const std::string &type, std::optional<Location> loc = std::nullopt);
    void DeleteFixture(FixtureId id);
    void SetName(FixtureId id, const std::string &name);
    void SetUniverse(FixtureId id, int v);
    void SetChannel(FixtureId id, int v);
    void SetLocation(FixtureId id, Location v);
    std::vector<FixtureId> ApplyChangesAndGetDeletions();
    void CalculateFixtureValues();
    void WriteValuesToUniverses(Artnet &, const std::vector<FixtureId> &ids);
    void CacheConfig();
};

inline Fixture *
FixtureManager::GetFixture(FixtureId id, bool onlyActive)
{
    auto find = this->fixtures.find(id);
    if (find != this->fixtures.end()) {
        if (onlyActive) {
            if (!find->second.Active()) {
                return nullptr;
            }
        }

        return &find->second;
    }

    return nullptr;
}