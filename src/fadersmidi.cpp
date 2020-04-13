#include "fadersmidi.hpp"

#include "helpers.hpp"

#include <iostream>

static const uint8_t type      = 3;
static const uint8_t channel   = 0;
static const uint8_t indexBase = 16;

MIDIFaders::MIDIFaders(MIDI &midi, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        auto subCallback = [&fader](uint8_t, uint8_t, uint8_t, uint8_t value) {
            float change = 0;

            if (value < 64) {
                change = -0.02;
            } else if (value > 64) {
                change = 0.02;
            }

            fader.SetRelative(change);
        };

        midi.Subscribe(type, channel, indexBase + i, subCallback);

        fader.Subscribe([&midi, i](float value, TimePoint) {
            uint8_t v = value * 15;
            midi.Send(type, channel, indexBase + i, v);
        });
    }
}
