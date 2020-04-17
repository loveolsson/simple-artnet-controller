#include "fadersosc.hpp"

#include "oschandler.hpp"

static std::vector<std::unique_ptr<OSCCallable>> subscriptions;

template <typename Func>
static OSCCallable *
CreateCallable(Func fn)
{
    auto callable = std::make_unique<OSCHandler<Func>>(fn);
    subscriptions.push_back(std::move(callable));

    return subscriptions.back().get();
}

void
InitOSCFaders(OSC &osc, FaderBank &faderBank)
{
    for (int i = 0; i < FaderBank::FaderCount; ++i) {
        auto &fader      = faderBank.GetFader(i);
        std::string path = "/fader/" + std::to_string(i);

        // Subscribe to OSC events for fader
        osc.AddTarget(path + "/off", CreateCallable([&fader]() { fader.SetValue(0); }));
        osc.AddTarget(path + "/on", CreateCallable([&fader]() { fader.SetValue(IntVal::Max); }));
        osc.AddTarget(path + "/value",
                      CreateCallable([&fader](const float v) { fader.SetValue(v); }));
        osc.AddTarget(path + "/nudge",
                      CreateCallable([&fader](const float v) { fader.SetRelative(v); }));
        osc.AddTarget(path + "/fadeto", CreateCallable([&fader](const float v, const float t) {
                          std::chrono::duration<float> seconds(t);
                          auto dur = std::chrono::duration_cast<TimePoint::duration>(seconds);
                          fader.FadeTo(v, dur);
                      }));
    }
}
