#pragma once
#include <nlohmann/json.hpp>

#include <chrono>
#include <optional>
#include <string>

using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

static inline TimePoint
NowAsTimePoint()
{
    return TimePoint{std::chrono::duration_cast<TimePoint::duration>(
        std::chrono::system_clock::now().time_since_epoch())};
}

static inline std::optional<TimePoint>
SafeParseJsonTimePointString(nlohmann::json j)
{
    try {
        return TimePoint{TimePoint::duration{std::stoll(j.get<std::string>())}};
    } catch (std::invalid_argument &) {
    } catch (std::out_of_range &) {
    } catch (nlohmann::json::exception &) {
    }

    return std::nullopt;
}

static inline TimePoint
SafeParseJsonTimePointString(nlohmann::json j, TimePoint fallback)
{
    try {
        return TimePoint{TimePoint::duration{std::stoll(j.get<std::string>())}};
    } catch (std::invalid_argument &) {
    } catch (std::out_of_range &) {
    } catch (nlohmann::json::exception &) {
    }

    return fallback;
}

template <typename T>
inline std::optional<T>
SafeParseJson(nlohmann::json j)
{
    try {
        return j.get<T>();
    } catch (nlohmann::json::exception &) {
    }
    return std::nullopt;
}

static inline std::string
TimePointToString(TimePoint t)
{
    return std::to_string(t.time_since_epoch().count());
}