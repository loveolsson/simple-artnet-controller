#include "artnet.hpp"

#include "artnet/artnet.h"

#include <algorithm>
#include <iostream>
#include <thread>

using namespace std::chrono;

static const high_resolution_clock::duration throttleInterval(milliseconds(25));  // 40 fps
static high_resolution_clock::time_point lastBroadcast(steady_clock::duration::zero());

void
Artnet::Start(const std::string &ip)
{
    this->node = artnet_new(ip.c_str(), 0);
    if (!this->node) {
        std::cerr << "Unable to set up artnet node:" << artnet_strerror() << std::endl;
        return;
    }

    artnet_set_short_name(node, "simple-artnet");
    artnet_set_long_name(node, "Simple Studio Artnet Controller");
    artnet_set_node_type(node, ARTNET_NODE);
    artnet_set_subnet_addr(node, 0x00);

    for (int i = 0; i < this->universes.size(); ++i) {
        artnet_set_port_type(node, i, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
        artnet_set_port_addr(node, i, ARTNET_INPUT_PORT, i);
    }

    if (artnet_start(node) != 0) {
        std::cerr << "Unable to start artnet node:" << artnet_strerror() << std::endl;
        this->Stop();
    }
}

void
Artnet::Stop()
{
    if (node) {
        artnet_stop(node);
        artnet_destroy(node);
        node = nullptr;
    }
}

void
Artnet::SendDMX()
{
    if (!this->node) {
        return;
    }

    int i = 0;
    for (auto &universe : this->universes) {
        artnet_send_dmx(this->node, i, universe.data.size(), universe.data.data());
        ++i;
    }
}

void
Artnet::WaitForNextFrame()
{
    auto now      = high_resolution_clock::now();
    auto earliest = lastBroadcast + throttleInterval;

    if (now < earliest) {
        auto wait = earliest - now;
        std::this_thread::sleep_for(wait);
        lastBroadcast = earliest;
    } else {
        lastBroadcast = now;
    }
}

void
Artnet::SetUniversesZero()
{
    for (auto &universe : this->universes) {
        universe.data.fill(0);
    }
}