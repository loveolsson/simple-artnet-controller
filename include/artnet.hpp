#pragma once
#include "fixture.hpp"

#include <array>
#include <chrono>
#include <string>

extern "C" {
#include "artnet/artnet.h"
}

struct ArtnetUniverse {
    std::array<uint8_t, 512> data;
    std::array<FixtureId, 512> fixtures;
};

class Artnet
{
    std::array<ArtnetUniverse, 4> universes;
    artnet_node node;
    std::chrono::steady_clock::time_point lastBroadcast;

public:
    Artnet(const std::string &ip);
    ~Artnet();

    ArtnetUniverse &GetUniverse(int index);
    void Send();
    void Throttle();
};