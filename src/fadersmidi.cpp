#include "fadersmidi.hpp"

#include "helpers.hpp"

#include <iostream>
#include <vector>

static const uint8_t type      = 3;
static const uint8_t channel   = 0;
static const uint8_t indexBase = 16;

class MidiSubscriber : public MidiUser
{
    Fader &fader;

public:
    MidiSubscriber(Fader &fader)
        : fader(fader)
    {
    }

    virtual void
    HandleData(uint8_t, uint8_t, uint8_t, uint8_t value) final
    {
        int change = 0;

        if (value < 64) {
            change = -IntVal::Max / 128;
        } else if (value > 64) {
            change = IntVal::Max / 128;
        }

        fader.SetRelative(change);
    }
};

static std::vector<std::unique_ptr<MidiSubscriber>> subscribers;

void
InitMIDIFaders(MIDI &midi, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader = faderBank.GetFader(i);
        auto user   = std::make_unique<MidiSubscriber>(fader);

        // Subscribe to MIDI events for fader
        midi.Subscribe(type, channel, indexBase + i, user.get());
        subscribers.push_back(std::move(user));

        // Subscribe to events from fader
        fader.Subscribe([&midi, i](IntVal value, TimePoint) {
            uint8_t v = value.Get01() * 15;
            midi.Send(type, channel, indexBase + i, v);
        });
    }
}
