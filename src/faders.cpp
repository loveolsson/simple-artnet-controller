#include "faders.hpp"

#include "helpers.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std::chrono;

static float
Lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void
Fader::SetValue(IntVal v)
{
    this->setQueue.enqueue({v, TimePoint::duration::zero(), false});
}

void
Fader::SetRelative(IntVal v)
{
    this->setQueue.enqueue({v, TimePoint::duration::zero(), true});
}

void
Fader::FadeTo(IntVal v, TimePoint::duration dur)
{
    this->setQueue.enqueue({v, dur, false});
}

void
Fader::Subscribe(std::function<void(IntVal, TimePoint)> fn)
{
    this->subscriptions.push_back(fn);
    SetRelative({0});
}

IntVal
Fader::Poll()
{
    FaderValueSet set;
    bool hasChanged = false;

    while (this->setQueue.try_dequeue(set)) {
        hasChanged = true;

        this->fadeTime = set.duration;
        if (set.duration > TimePoint::duration::zero()) {
            this->fromValue = this->value;
            this->fadeStart = NowAsTimePoint();

            if (set.isRelative) {
                this->toValue.val = std::clamp((this->value.val + set.v.val), 0, IntVal::Max);
            } else {
                this->toValue.val = std::clamp(set.v.val, 0, IntVal::Max);
            }
        } else {
            if (set.isRelative) {
                this->value.val = std::clamp((this->value.val + set.v.val), 0, IntVal::Max);
            } else {
                this->value.val = std::clamp(set.v.val, 0, IntVal::Max);
            }
        }
    }

    if (!hasChanged && this->fadeTime > TimePoint::duration::zero()) {
        auto now     = NowAsTimePoint();
        auto endTime = this->fadeStart + this->fadeTime;

        if (now > endTime) {
            this->fadeTime = TimePoint::duration::zero();
            this->value    = this->toValue;
        } else {
            auto timeIntoFade = (float)(now - this->fadeStart).count() / this->fadeTime.count();

            this->value =
                IntVal(Lerp(this->fromValue.Get01(), this->toValue.Get01(), timeIntoFade));
        }

        hasChanged = true;
    }

    if (hasChanged) {
        auto changed = NowAsTimePoint();
        for (auto &fn : this->subscriptions) {
            fn(this->value, changed);
        }
    }

    return this->value;
}

void
Fader::LoadState(Settings &settings, int index)
{
    this->value.val = settings.GetJSON("faderValue" + std::to_string(index), 0);
}

void
Fader::SaveState(Settings &settings, int index)
{
    settings.SetJSON("faderValue" + std::to_string(index), this->value.val);
}

void
FaderBank::LoadSettings(Settings &settings)
{
    int i = 0;
    for (auto &fader : this->faders) {
        fader.LoadState(settings, i);
        ++i;
    }
}

void
FaderBank::StoreSettings(Settings &settings)
{
    int i = 0;
    for (auto &fader : this->faders) {
        fader.SaveState(settings, i);
        ++i;
    }
}

std::array<IntVal, FaderBank::FaderCount>
FaderBank::ApplyChangesAndGetState()
{
    std::array<IntVal, FaderBank::FaderCount> res;

    int i = 0;
    for (auto &fader : this->faders) {
        res[i++] = fader.Poll();
    }

    return res;
}