#include "fadersmidi.hpp"

#include "helpers.hpp"

#include <iostream>
#include <vector>

/**
 * MIDI parameters
 * TODO: Make configurable
 */
#define MIDI_MSG_TYPE 3
#define MIDI_CHANNEL 0
#define MIDI_CTRL_START 0
#define MIDI_LO_OFFSET 32
#define MIDI_FADER_SWITCH_INDEX 15
#define MIDI_FADER_SWITCH_STATE 47

/**
 * The ratio between midi values and IntVal::Max
 */
#define VALUE_RATIO (IntVal::Max / 0x3fff)

class MidiSubscriber : public MidiUser
{
    Fader &fader;
    MIDI &midi;

    uint16_t tempVal    = 0;
    int valCount        = 0;
    uint8_t lastSwIndex = 0xFF;
    bool pressed        = false;
    uint16_t lastSetVal = 0;

public:
    const uint8_t loIndex;
    const uint8_t hiIndex;
    const int index;

    MidiSubscriber(Fader &fader, MIDI &midi, int index, uint8_t loIndex, uint8_t hiIndex)
        : fader(fader)
        , midi(midi)
        , loIndex(loIndex)
        , hiIndex(hiIndex)
        , index(index)
    {
    }

    virtual void
    HandleData(uint8_t, uint8_t, uint8_t ch, uint8_t value) final
    {
        if (ch == loIndex) {
            this->tempVal |= value;
            this->valCount++;
        } else if (ch == hiIndex) {
            this->tempVal |= (uint16_t)value << 7;
            this->valCount++;
        } else if (ch == MIDI_FADER_SWITCH_INDEX) {
            this->lastSwIndex = value;
        } else if (ch == MIDI_FADER_SWITCH_STATE && this->lastSwIndex == this->index) {
            this->pressed = value > 0;
            if (!this->pressed) {
                this->SetValue(this->lastSetVal);
            }
        }

        if (this->valCount == 2) {
            fader.SetValue(this->tempVal * VALUE_RATIO);
            this->tempVal  = 0;
            this->valCount = 0;
        }
    }

    void
    SetValue(uint16_t val)
    {
        this->lastSetVal = val;

        if (!this->pressed) {
            uint16_t v     = val / VALUE_RATIO;
            uint8_t loByte = v & 0x7f;
            uint8_t hiByte = (v >> 7) & 0x7f;

            midi.Send(MIDI_MSG_TYPE, MIDI_CHANNEL, hiIndex, hiByte);
            midi.Send(MIDI_MSG_TYPE, MIDI_CHANNEL, loIndex, loByte);
        }
    }
};

static std::vector<std::unique_ptr<MidiSubscriber>> subscribers;

void
InitMIDIFaders(MIDI &midi, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader = faderBank.GetFader(i);

        uint8_t hiIndex = MIDI_CTRL_START + i;
        uint8_t loIndex = hiIndex + MIDI_LO_OFFSET;

        auto user = std::make_unique<MidiSubscriber>(fader, midi, i, loIndex, hiIndex);

        // Subscribe to MIDI events for fader
        midi.Subscribe(MIDI_MSG_TYPE, MIDI_CHANNEL, hiIndex, -1, user.get());
        midi.Subscribe(MIDI_MSG_TYPE, MIDI_CHANNEL, loIndex, -1, user.get());
        midi.Subscribe(MIDI_MSG_TYPE, MIDI_CHANNEL, MIDI_FADER_SWITCH_INDEX, -1, user.get());
        midi.Subscribe(MIDI_MSG_TYPE, MIDI_CHANNEL, MIDI_FADER_SWITCH_STATE, -1, user.get());

        // Subscribe to events from fader
        fader.Subscribe([&midi, i, &u = *user](FaderState state) { u.SetValue(state.value.val); });

        subscribers.push_back(std::move(user));

        std::vector<uint8_t> data = {
            0xf0, 0x00, 0x00, 0x66, 0x05, 0x00, 0x10, (uint8_t)i, 'S', 'C', ' ', (uint8_t)('0' + i),
            0xf7,
        };

        midi.Send(data);
    }
}
