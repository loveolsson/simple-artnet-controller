#pragma once
#include <array>
#include <chrono>
#include <string>

typedef void *artnet_node;

struct ArtnetUniverse {
    std::array<uint8_t, 512> data;
};

class Artnet
{
    std::array<ArtnetUniverse, 4> universes;
    artnet_node node;

public:
    void Start(const std::string &ip);
    void Stop();

    ArtnetUniverse *GetUniverse(size_t index);
    void SendDMX();
    void SetUniversesZero();

    static void WaitForNextFrame();
};

inline ArtnetUniverse *
Artnet::GetUniverse(size_t index)
{
    if (index < this->universes.size()) {
        return &this->universes[index];
    }

    return nullptr;
}