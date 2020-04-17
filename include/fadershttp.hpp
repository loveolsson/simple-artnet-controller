#pragma once
#include "faders.hpp"
#include "helpers.hpp"
#include "httpserver.hpp"

#include <array>
#include <atomic>

class HTTPFaders
{
    std::array<std::atomic<uint16_t>, FaderBank::FaderCount> faderValues{0};
    std::atomic<TimePoint> lastChange{TimePoint{TimePoint::duration::zero()}};

public:
    HTTPFaders(HTTPServer &server, FaderBank &faderBank);
};