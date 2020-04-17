#include "fixturemanagerhttp.hpp"

#include "helpers.hpp"

using namespace nlohmann;

void
InitFixtureManagerHTTP(HTTPServer &server, FixtureManager &fixtureManager)
{
    server.AttachJSONGet("/fixtures", [&fixtureManager](auto req) {
        TimePoint since =
            SafeParseJsonTimePointString(req["since"], TimePoint{TimePoint::duration::zero()});

        auto fixtures = fixtureManager.jsonConfigCache.GetData(since);
        if (fixtures.first.has_value()) {
            return std::make_tuple(
                std::move(*fixtures.first), 200,
                HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}});
        }

        return std::make_tuple(
            json(), 304, HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}});
    });

    server.AttachJSONGet("/fixtureDescriptions", [&fixtureManager](auto req) {
        TimePoint since =
            SafeParseJsonTimePointString(req["since"], TimePoint{TimePoint::duration::zero()});

        auto fixtures = fixtureManager.jsonFixtureDescs.GetData(since);
        if (fixtures.first.has_value()) {
            return std::make_tuple(
                std::move(*fixtures.first), 200,
                HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}});
        }

        return std::make_tuple(
            json(), 304, HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}});
    });
}