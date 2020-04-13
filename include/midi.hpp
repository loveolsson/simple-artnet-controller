#pragma once

#include <rtmidi/RtMidi.h>

#include <functional>
#include <memory>
#include <vector>

struct MidiSub {
    int type;
    int channel;
    int index;
    std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> fn;
};

class MIDI
{
    std::unique_ptr<RtMidiIn> input;
    std::unique_ptr<RtMidiOut> output;

    std::vector<MidiSub> subscriptions;

public:
    MIDI();
    ~MIDI();

    bool Valid();
    void Poll();
    void Subscribe(int type, int channel, int index,
                   std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> fn);
    void Send(uint8_t type, uint8_t channel, uint8_t index, uint8_t value);
};