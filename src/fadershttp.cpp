#include "fadershttp.hpp"

#include "helpers.hpp"

using namespace nlohmann;

HTTPFaders::HTTPFaders(HTTPServer &server, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        auto &valueCache = this->faderValues[i];
        auto &changed    = this->lastChange;

        fader.Subscribe([&valueCache, &changed](IntVal value, TimePoint tp) {
            valueCache = value.val;
            changed    = tp;
        });
    }

    server.AttachJSONGet("/faders", [this](auto &req) {
        TimePoint since{TimePoint::duration::zero()};

        auto s = SafeParseJsonTimePointString(req["since"]);
        if (s.has_value()) {
            since = *s;
        }

        auto changed = this->lastChange.load();
        if (since < changed) {
            auto arr = json::array();
            for (int i = 0; i < FaderBank::FaderCount; ++i) {
                auto &valueCache = this->faderValues[i];
                arr.push_back(json{
                    {"value", valueCache.load()},
                });
            }
            json j = json{
                {"faders", std::move(arr)},
            };

            return std::make_tuple(std::move(j), 200,
                                   HTTPHeaders{{"SimpleArtnetTime", TimePointToString(changed)}});
        }

        return std::make_tuple(json{}, 304,
                               HTTPHeaders{{"SimpleArtnetTime", TimePointToString(changed)}});
    });

    server.AttachJSONPost("/faders", [&faderBank](auto &req) {
        auto index = SafeParseJson<int>(req["index"]);
        auto value = SafeParseJson<int>(req["value"]);

        if (!index.has_value() || !value.has_value()) {
            return std::make_tuple(json{}, 500, HTTPHeaders{});
        }

        auto &fader = faderBank.GetFader(*index);
        fader.SetValue({*value});
        return std::make_tuple(json{}, 200, HTTPHeaders{});
    });
}