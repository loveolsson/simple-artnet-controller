#pragma once
#include "helpers.hpp"
#include "settings.hpp"
#include "types.hpp"

#include <concurrentqueue.h>

#include <array>
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>

struct FaderValueSet {
    IntVal v;
    TimePoint::duration duration;
    bool isRelative;
};

class Fader
{
    moodycamel::ConcurrentQueue<FaderValueSet> setQueue;
    std::vector<std::function<void(IntVal, TimePoint)>> subscriptions;

    IntVal fromValue, toValue;
    IntVal value;
    TimePoint fadeStart;
    TimePoint::duration fadeTime = TimePoint::duration::zero();

public:
    void SetValue(IntVal v);
    void SetRelative(IntVal v);
    void FadeTo(IntVal v, TimePoint::duration dur);
    void Subscribe(std::function<void(IntVal, TimePoint)> fn);
    IntVal Poll();
    void LoadState(Settings &, int index);
    void SaveState(Settings &, int index);
};

class FaderBank
{
public:
    static constexpr size_t FaderCount = 8;

private:
    std::array<Fader, FaderBank::FaderCount> faders;

public:
    void LoadSettings(Settings &);
    void StoreSettings(Settings &);

    Fader &GetFader(int i);
    std::array<IntVal, FaderBank::FaderCount> ApplyChangesAndGetState();
};

inline Fader &
FaderBank::GetFader(int i)
{
    auto iClamped = std::clamp(i, 0, (int)this->faders.size() - 1);
    return this->faders[iClamped];
}
