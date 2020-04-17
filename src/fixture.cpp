#include "fixture.hpp"

#include "typesjson.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace nlohmann;

Fixture::Fixture(const std::string &type, FixtureId id, const json &fixtureDesc, const json &config)
    : id(id)
    , type(type)
{
    this->parameterLookup.fill(0);
    this->parameterLookup[(size_t)Parameters::One]  = 1;
    this->parameterLookup[(size_t)Parameters::Full] = 255;

    if (!config.is_null()) {
        this->Config(config);
    }

    {
        std::vector<std::string> pS = fixtureDesc["provides"];

        auto Has = [&pS](const std::string &str) {
            return std::find(pS.cbegin(), pS.cend(), str) != pS.cend();
        };

        this->provides.dimmer      = Has("dimmer");
        this->provides.color       = Has("color");
        this->provides.temperature = Has("temperature");
    }

    {
        std::vector<std::string> cS = fixtureDesc["channels"];

        for (const auto &channel : cS) {
            auto p = LookupParameterName(channel);

            this->parameters.push_back(p);
        }
    }

    if (this->provides.color) {
        std::string mS = fixtureDesc["color_matrix"];

        this->colorMatrix = LookupColorMatrixName(mS);
        if (this->colorMatrix == ColorMatrix::None) {
            throw std::invalid_argument(
                "Expected color matrix for fixture since it provides color");
        }
    }
}

void
Fixture::SetIntensity(float v)
{
    this->intensity = v;
}

void
Fixture::SetColor(const ColorRGB &v)
{
    this->color = v;
}

void
Fixture::Calculate()
{
    if (this->provides.dimmer) {
        parameterLookup[(int)Parameters::Dimmer] = this->intensity * 255;
    }

    if (this->provides.color) {
        switch (this->colorMatrix) {
            case ColorMatrix::RGB: {
                parameterLookup[(size_t)Parameters::Red]   = color.r * 255;
                parameterLookup[(size_t)Parameters::Green] = color.g * 255;
                parameterLookup[(size_t)Parameters::Blue]  = color.b * 255;
                break;
            }
            case ColorMatrix::DimRGB: {
                parameterLookup[(size_t)Parameters::Red]   = intensity * color.r * 255;
                parameterLookup[(size_t)Parameters::Green] = intensity * color.g * 255;
                parameterLookup[(size_t)Parameters::Blue]  = intensity * color.b * 255;
                break;
            }

            default: {
                throw std::invalid_argument("Color matrix not implemented");
            }
        }
    }
}

void
Fixture::WriteValuesToUniverse(Artnet &artnet)
{
    if (this->universe < 0 || this->channel < 0) {
        return;
    }

    auto universe = artnet.GetUniverse(this->universe);
    if (!universe) {
        return;
    }

    int i = this->channel;

    for (auto &parameter : this->parameters) {
        if (i < universe->data.size()) {
            universe->data[i++] = this->parameterLookup[(int)parameter];
        }
    }
}

void
Fixture::Config(const json &config)
{
    auto GetIfAvail = [&config](auto &v, const std::string &name) {
        auto find = config.find(name);
        if (find != config.end()) {
            try {
                v = *find;
            } catch (json::exception &) {
            }
        }
    };

    GetIfAvail(this->name, "name");
    GetIfAvail(this->universe, "universe");
    GetIfAvail(this->channel, "channel");
    GetIfAvail(this->location, "location");
}

json
Fixture::SerializeConfig()
{
    return json{{"name", this->name},
                {"universe", this->universe},
                {"channel", this->channel},
                {"location", json{
                                 {"x", this->location.x},
                                 {"y", this->location.y},
                             }}};
}

Fixture::Parameters
Fixture::LookupParameterName(const std::string &str)
{
    if (str == "one") {
        return Parameters::One;
    } else if (str == "dimmer") {
        return Parameters::Dimmer;
    } else if (str == "red") {
        return Parameters::Red;
    } else if (str == "green") {
        return Parameters::Green;
    } else if (str == "blue") {
        return Parameters::Blue;
    } else if (str == "ww") {
        return Parameters::WW;
    } else if (str == "cw") {
        return Parameters::CW;
    } else {
        return Parameters::Zero;
    }
}

Fixture::ColorMatrix
Fixture::LookupColorMatrixName(const std::string &str)
{
    if (str == "rgb") {
        return ColorMatrix::RGB;
    } else if (str == "rgba") {
        return ColorMatrix::RGBA;
    } else if (str == "rgbw") {
        return ColorMatrix::RGBW;
    } else if (str == "dim_rgb") {
        return ColorMatrix::DimRGB;
    } else if (str == "dim_rgba") {
        return ColorMatrix::DimRGBA;
    } else if (str == "dim_rgbw") {
        return ColorMatrix::DimRGBW;
    } else if (str == "temp_approx") {
        return ColorMatrix::TempApprox;
    } else if (str == "dim_temp_approx") {
        return ColorMatrix::DimTempApprox;
    } else {
        return ColorMatrix::None;
    }
}
