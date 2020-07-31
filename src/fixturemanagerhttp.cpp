#include "fixturemanagerhttp.hpp"

#include "helpers.hpp"
#include "typesjson.hpp"

#include <iostream>

using namespace nlohmann;

void
InitFixtureManagerHTTP(HTTPServer &server, FixtureManager &fixtureManager)
{
    fixtureManager.jsonConfigCache.Serve(server, "/fixtures");
    fixtureManager.jsonFixtureDescs.Serve(server, "/fixtureDescriptions");

    server.AttachJSONPost("/setFixtureLocation", [&fixtureManager](HTTPJsonPostReq req) {
        try {
            FixtureId id = std::stoull(req["id"].get<std::string>());
            Location loc = req["location"];

            fixtureManager.SetLocation(id, loc);
        } catch (nlohmann::json::exception &ex) {
            return HTTPJsonRes{nullptr, restinio::status_internal_server_error(), {}};
        }

        return HTTPJsonRes{nullptr, restinio::status_ok(), {}};
    });

    server.AttachJSONPost("/createFixture", [&fixtureManager](HTTPJsonPostReq req) {
        try {
            std::string type = req["type"];
            Location loc     = req["location"];

            fixtureManager.CreateFixture(type, loc);
        } catch (nlohmann::json::exception &ex) {
            return HTTPJsonRes{nullptr, restinio::status_internal_server_error(), {}};
        }

        return HTTPJsonRes{nullptr, restinio::status_ok(), {}};
    });
}