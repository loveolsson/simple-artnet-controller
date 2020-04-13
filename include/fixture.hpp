#pragma once

#include <nlohmann/json.hpp>

#include <cstdint>

using FixtureId = uint64_t;

class Fixture
{
public:
    Fixture(FixtureId id, const nlohmann::json &fixtureDesc, const nlohmann::json &config);

    const FixtureId id;

    void Config(const nlohmann::json &config);
    nlohmann::json SerializeConfig();
};