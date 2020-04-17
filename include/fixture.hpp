#pragma once

#include "artnet.hpp"
#include "types.hpp"

#include <nlohmann/json_fwd.hpp>

#include <array>
#include <cstdint>
#include <vector>

using FixtureId = uint64_t;

class Fixture
{
public:
    struct Provide {
        bool dimmer : 1;
        bool color : 1;
        bool temperature : 1;
    };

private:
    enum class Parameters {
        Zero = 0,
        One,
        Full,
        Dimmer,
        Red,
        Green,
        Blue,
        WW,
        CW,
        Count,
    };

    enum class ColorMatrix {
        None = 0,
        RGB,
        RGBA,
        RGBW,
        DimRGB,
        DimRGBA,
        DimRGBW,
        TempApprox,
        DimTempApprox,
    } colorMatrix = ColorMatrix::None;

    Provide provides{};

    Location location{-1, -1};
    int universe = -1;
    int channel  = -1;

    float intensity = 0;
    ColorRGB color{};

    std::array<uint8_t, (size_t)Parameters::Count> parameterLookup;
    std::vector<Parameters> parameters;

    static Parameters LookupParameterName(const std::string &);
    static ColorMatrix LookupColorMatrixName(const std::string &);

    const std::string type;
    std::string name;

public:
    Fixture(const std::string &type, FixtureId id, const nlohmann::json &fixtureDesc,
            const nlohmann::json &config);

    const FixtureId id;

    void SetIntensity(float v);
    void SetColor(const ColorRGB &v);
    void Calculate();
    void WriteValuesToUniverse(Artnet &);

    void Config(const nlohmann::json &config);
    nlohmann::json SerializeConfig();

    bool Active() const;
    Provide Provides() const;
    const std::string &Type() const;
    const std::string &Name() const;
};

bool inline Fixture::Active() const
{
    return this->location.x != -1 && this->location.y != -1;
}

inline Fixture::Provide
Fixture::Provides() const
{
    return this->provides;
}

inline const std::string &
Fixture::Type() const
{
    return this->type;
}

inline const std::string &
Fixture::Name() const
{
    return this->name;
}