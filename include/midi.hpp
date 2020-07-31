#pragma once

#include <memory>
#include <vector>

class RtMidiIn;
class RtMidiOut;

class MidiUser
{
public:
    virtual void HandleData(uint8_t, uint8_t, uint8_t, uint8_t) = 0;
};

struct MidiSub {
    int type;
    int channel;
    int index;
    int value;
    MidiUser *user;
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
    void PollEvents();
    void Subscribe(int type, int channel, int index, int value, MidiUser *user);
    void Send(uint8_t type, uint8_t channel, uint8_t index, uint8_t value);
    void Send(const std::vector<uint8_t> &data);
};