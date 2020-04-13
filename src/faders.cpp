#include "faders.hpp"

#include "helpers.hpp"

#include <algorithm>
#include <iostream>

using namespace std::chrono;

Fader::Fader(Settings &settings, int index)
    : settings(settings)
    , value(0)
    , index(index)
{
    this->value = this->settings.GetFloat("faderValue" + std::to_string(this->index), 0);
}

Fader::~Fader()
{
    this->settings.SetFloat("faderValue" + std::to_string(this->index), this->value);
}

void
Fader::SetValue(float v)
{
    this->setQueue.enqueue({v, false});
}

void
Fader::SetRelative(float v)
{
    this->setQueue.enqueue({v, true});
}

void
Fader::Subscribe(std::function<void(float, TimePoint)> fn)
{
    this->subscriptions.push_back(fn);
    SetRelative(0);
}

void
Fader::Poll()
{
    FaderValueSet set;
    bool hasChanged = false;
    while (this->setQueue.try_dequeue(set)) {
        hasChanged = true;
        if (set.isRelative) {
            this->value = std::clamp(this->value + set.value, 0.f, 1.f);
        } else {
            this->value = std::clamp(set.value, 0.f, 1.f);
        }
    }

    if (hasChanged) {
        auto changed = NowAsTimePoint();
        for (auto &fn : this->subscriptions) {
            fn(this->value, changed);
        }
    }
}

FaderBank::FaderBank(Settings &settings)
{
    for (int i = 0; i < FaderCount; ++i) {
        this->faders[i] = std::make_unique<Fader>(settings, i);
    }
}

Fader &
FaderBank::GetFader(int i)
{
    auto iClamped = std::clamp(i, 0, (int)this->faders.size() - 1);
    return *this->faders[iClamped];
}

void
FaderBank::Poll()
{
    for (auto &fader : this->faders) {
        fader->Poll();
    }
}