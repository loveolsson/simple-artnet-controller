#include "midi.hpp"

#include <rtmidi/RtMidi.h>

#include <iostream>

MIDI::MIDI()
{
    this->input  = std::make_unique<RtMidiIn>();
    this->output = std::make_unique<RtMidiOut>();

    auto inputCount  = this->input->getPortCount();
    auto outputCount = this->output->getPortCount();

    if (inputCount && outputCount) {
        this->input->openPort(0);
        this->output->openPort(0);
    }

    if (this->Valid()) {
        this->input->ignoreTypes(false, false, false);
    }
}

MIDI::~MIDI()
{
    this->input->closePort();
    this->output->closePort();
}

bool
MIDI::Valid()
{
    return this->input->isPortOpen() && this->output->isPortOpen();
}

void
MIDI::PollEvents()
{
    if (!this->Valid()) {
        return;
    }

    std::vector<uint8_t> msg;
    while (this->input->getMessage(&msg) >= 0 && msg.size() > 0) {
        if (msg.size() == 3) {
            uint8_t type    = (msg[0] >> 4) & 0x07;
            uint8_t channel = msg[0] & 0x0F;
            uint8_t index   = msg[1] & 0x7F;
            uint8_t value   = msg[2] & 0x7F;

            for (auto &s : this->subscriptions) {
                if ((s.type == type || s.type < 0) && (s.channel == channel || s.channel < 0) &&
                    (s.index == index || s.index < 0)) {
                    s.user->HandleData(type, channel, index, value);
                }
            }
        }
    }
}

void
MIDI::Subscribe(int type, int channel, int index, MidiUser *user)
{
    this->subscriptions.push_back({type, channel, index, user});
}

void
MIDI::Send(uint8_t type, uint8_t channel, uint8_t index, uint8_t value)
{
    if (!this->output || !this->output->isPortOpen()) {
        return;
    }

    std::vector<uint8_t> msg = {
        (uint8_t)(0x80 | ((type & 0x07) << 4) | (channel & 0x0F)),
        (uint8_t)(index & 0x7F),
        (uint8_t)(value & 0x7F),
    };

    this->output->sendMessage(&msg);
}
