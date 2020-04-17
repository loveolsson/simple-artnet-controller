#include "fixturemanagerhttp.hpp"

#include "helpers.hpp"

#include <iostream>

using namespace nlohmann;

void
InitFixtureManagerHTTP(HTTPServer &server, FixtureManager &fixtureManager)
{
    server.AttachJSONGetAsync("/fixtures", [&fixtureManager](auto req, auto resCb) {
        TimePoint since =
            SafeParseJsonTimePointString(req["since"], TimePoint{TimePoint::duration::zero()});

        fixtureManager.jsonConfigCache.GetDataAsync(since, [resCb](auto fixtures) {
            std::cout << "Async" << std::endl;
            if (fixtures.first.has_value()) {
                resCb(std::make_tuple(
                    std::move(*fixtures.first), 200,
                    HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}}));
            }

            resCb(std::make_tuple(
                json(), 304,
                HTTPHeaders{{"SimpleArtnetTime", TimePointToString(fixtures.second)}}));
        });
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