#pragma once

#include "faders.hpp"
#include "midi.hpp"

class MIDIFaders
{
public:
    MIDIFaders(MIDI &midi, FaderBank &faderBank);
};
