#include "artnet.hpp"

#include <algorithm>
#include <iostream>
#include <thread>

using namespace std::chrono;

static const steady_clock::duration throttleInterval(milliseconds(25));

Artnet::Artnet(const std::string &ip)
    : lastBroadcast(steady_clock::duration::zero())
{
    this->node = artnet_new(ip.c_str(), 0);
    if (!this->node) {
        std::cout << "Unable to set up artnet node:" << artnet_strerror() << std::endl;
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
        std::cout << "Unable to start artnet node:" << artnet_strerror() << std::endl;
        artnet_destroy(node);
        node = nullptr;
    }
}

Artnet::~Artnet()
{
    if (node) {
        artnet_stop(node);
        artnet_destroy(node);
    }
}

ArtnetUniverse &
Artnet::GetUniverse(int index)
{
    index = std::clamp(index, 0, (int)this->universes.size() - 1);
    return this->universes[index];
}

void
Artnet::Send()
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
Artnet::Throttle()
{
    auto now      = steady_clock::now();
    auto earliest = this->lastBroadcast + throttleInterval;

    if (now < earliest) {
        auto wait = earliest - now;
        std::this_thread::sleep_for(wait);
        this->lastBroadcast = earliest;
    } else {
        this->lastBroadcast = now;
    }
}