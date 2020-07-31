#include "fadershttp.hpp"

#include "helpers.hpp"

using namespace nlohmann;

void
HTTPFaders::Cache()
{
    json j;

    auto &arr = j["faders"] = json::array();
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &valueCache = this->faderValues[i];
        arr.push_back(json{
            {"value", valueCache.load()},
        });
    }

    this->jsonCache.SetData(std::move(j));
}

HTTPFaders::HTTPFaders(HTTPServer &server, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        auto &valueCache = this->faderValues[i];
        auto &changed    = this->lastChange;

        fader.Subscribe([this, &valueCache, &changed](FaderState state) {
            valueCache = state.value.val;
            changed    = state.changed;
            this->Cache();
        });
    }

    this->jsonCache.Serve(server, "/faders");

    server.AttachJSONPost("/faders", [&faderBank](auto &req) {
        std::cout << "Foo";
        auto index = SafeParseJson<int>(req["index"]);
        auto value = SafeParseJson<int>(req["value"]);

        if (!index.has_value() || !value.has_value()) {
            return std::make_tuple(json{}, restinio::status_internal_server_error(), HTTPHeaders{});
        }

        auto &fader = faderBank.GetFader(*index);
        fader.SetValue({*value});
        return std::make_tuple(json{}, restinio::status_ok(), HTTPHeaders{});
    });
}

void
HTTPFaders::Poll()
{
    this->jsonCache.Poll();
}