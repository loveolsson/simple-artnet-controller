#pragma once
#include "fixture.hpp"

#include <concurrentqueue.h>
#include <nlohmann/json.hpp>

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

using ArrStr = std::array<char, 64>;

class FixtureManager
{
    std::map<std::string, nlohmann::json> fixtureDescs;
    std::vector<std::shared_ptr<Fixture>> fixtures;

    FixtureId FindFreeId();

    moodycamel::ConcurrentQueue<ArrStr> createQueue;

    void CreateFixtureInternal(const std::string &type);

public:
    FixtureManager();
    std::shared_ptr<Fixture> GetFixture(FixtureId);

    void CreateFixture(const std::string &type);
    void Poll();
};