#include "fadersosc.hpp"

OSCFaders::OSCFaders(OSC &osc, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        std::string path = "/fader/" + std::to_string(i) + "/value";

        osc.AddTarget(path, [&fader](const float v) { fader.SetValue(v); });
    }
}
