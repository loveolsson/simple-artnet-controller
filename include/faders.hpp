#pragma once
#include "helpers.hpp"
#include "settings.hpp"

#include <concurrentqueue.h>

#include <array>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

struct FaderValueSet {
    float value;
    bool isRelative;
};

class Fader
{
    moodycamel::ConcurrentQueue<FaderValueSet> setQueue;
    float value;
    std::vector<std::function<void(float, TimePoint)>> subscriptions;
    Settings &settings;
    const int index;

public:
    Fader(Settings &settings, int index);
    ~Fader();

    void SetValue(float v);
    void SetRelative(float v);
    void Subscribe(std::function<void(float, TimePoint)> fn);
    void Poll();
};

class FaderBank
{
public:
    static constexpr size_t FaderCount = 8;

private:
    std::array<std::unique_ptr<Fader>, FaderBank::FaderCount> faders;

public:
    FaderBank(Settings &);

    Fader &GetFader(int i);
    void Poll();
};