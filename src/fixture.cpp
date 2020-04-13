#include "fixture.hpp"

using namespace nlohmann;

Fixture::Fixture(FixtureId id, const json &fixtureDesc, const json &config)
    : id(id)
{
    if (!config.is_null()) {
        this->Config(config);
    }
}

void
Fixture::Config(const json &config)
{
}

json
Fixture::SerializeConfig()
{
    return json{};
}