#pragma once
#include "faders.hpp"
#include "helpers.hpp"
#include "httpserver.hpp"
#include "jsoncache.hpp"

#include <array>
#include <atomic>

class HTTPFaders
{
    std::array<std::atomic<uint16_t>, FaderBank::FaderCount> faderValues{0};
    std::atomic<TimePoint> lastChange{TimePoint{TimePoint::duration::zero()}};

    JSONCache jsonCache{TimePoint::duration{5000}};

    void Cache();

public:
    HTTPFaders(HTTPServer &server, FaderBank &faderBank);
    void Poll();
};