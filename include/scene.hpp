#pragma once

#include "fixture.hpp"
#include "fixturemanager.hpp"
#include "jsoncache.hpp"
#include "typesjson.hpp"

#include <concurrentqueue.h>

#include <map>

struct SceneMember {
    IntVal intensity   = -1;
    IntVal temperature = -1;
    int16_t colorIndex = -1;

    bool Active();
};

class Scene
{
    using ArrStr = std::array<char, 64>;

    struct Instruction {
        bool setIntensity;
        bool setTemperature;
        bool setColorIndex;
        bool setName;
        FixtureId id;
        IntVal intVal;
        ArrStr str;
    };

    std::map<FixtureId, SceneMember> members;
    moodycamel::ConcurrentQueue<Instruction> instructionQueue;

    SceneMember *GetMember(FixtureId);
    SceneMember *GetOrCreateMember(FixtureId);

    std::string name;

public:
    void RemoveFixture(FixtureId);

    void SetIntensity(FixtureId, IntVal);
    void SetTemperature(FixtureId, IntVal);
    void SetColorIndex(FixtureId, int16_t);
    void SetName(FixtureId, const std::string &);
    const std::map<FixtureId, SceneMember> &GetMembers();
    void LoadSettings(Settings &, FixtureManager &, int index);
    void SaveSettings(Settings &, int index);
    void Poll(FixtureManager &);
    void CacheConfig();
};

inline bool
SceneMember::Active()
{
    return intensity.val >= 0 || temperature.val >= 0 || colorIndex >= 0;
}

namespace nlohmann {
template <>
struct adl_serializer<SceneMember> {
    static void
    to_json(json &j, const SceneMember &value)
    {
        j["intensity"]   = value.intensity;
        j["temperature"] = value.temperature;
        j["colorIndex"]  = value.colorIndex;
    }

    static void
    from_json(const json &j, SceneMember &value)
    {
        value.intensity   = j["intensity"];
        value.temperature = j["temperature"];
        value.colorIndex  = j["colorIndex"];
    }
};
}  // namespace nlohmann
