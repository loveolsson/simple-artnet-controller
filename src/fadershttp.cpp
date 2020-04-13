#include "fadershttp.hpp"

#include "helpers.hpp"

using namespace nlohmann;

HTTPFaders::HTTPFaders(HTTPServer &server, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        auto &valueCache = this->faderValues[i];
        auto &changed    = this->lastChange;

        fader.Subscribe([&valueCache, &changed](float value, TimePoint tp) {
            valueCache = value;
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
                {"changed", TimePointToString(changed)},
                {"faders", std::move(arr)},
            };

            return std::make_pair(std::move(j), 200);
        }

        return std::make_pair(json{}, 304);
    });

    server.AttachJSONPost("/faders", [&faderBank](auto &req) {
        auto index = SafeParseJson<int>(req["index"]);
        auto value = SafeParseJson<float>(req["value"]);

        if (!index.has_value() || !value.has_value()) {
            return std::make_pair(json{}, 500);
        }

        auto &fader = faderBank.GetFader(*index);
        fader.SetValue(*value);
        return std::make_pair(json{}, 200);
    });
}