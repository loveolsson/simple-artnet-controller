#include "scenemanager.hpp"

#include "typesjson.hpp"

void
SceneManager::LoadSettings(Settings &settings, FixtureManager &fixtureManager)
{
    int i = 0;
    for (auto &scene : this->scenes) {
        scene.LoadSettings(settings, fixtureManager, i++);
    }
}

void
SceneManager::StoreSettings(Settings &settings)
{
    int i = 0;
    for (auto &scene : this->scenes) {
        scene.SaveSettings(settings, i++);
    }
}

void
SceneManager::ApplyChanges(FixtureManager &fixtureManager)
{
    int i = 0;
    for (auto &scene : this->scenes) {
        scene.Poll(fixtureManager);
    }
}

void
SceneManager::ScrubDeleatedFixtures(const std::vector<FixtureId> &deletions)
{
    for (auto &scene : this->scenes) {
        for (auto d : deletions) {
            scene.RemoveFixture(d);
        }
    }
}

struct ResolveMember {
    float intensity;
    float temperature;
    ColorRGB color;
    float colorAcc;
    float temperatureAcc;
};

std::vector<FixtureId>
SceneManager::ResolveScenes(const std::array<IntVal, 8> &faderValues,
                            FixtureManager &fixtureManager)
{
    std::map<FixtureId, ResolveMember> resolved;

    int i = 0;
    for (auto &scene : this->scenes) {
        float fader = faderValues[i++].Get01();

        for (auto &m : scene.GetMembers()) {
            auto id      = m.first;
            auto &member = m.second;

            ResolveMember newR{};
            newR.intensity = member.intensity.Get01() * fader;

            if (member.colorIndex >= 0) {
                newR.color    = {0.5f * fader, 0.5f * fader, 0.5f * fader};
                newR.colorAcc = fader;
            }

            auto r = resolved.find(id);
            if (r == resolved.end()) {
                resolved.insert(std::make_pair(id, newR));
                continue;
            }

            auto &rMember = r->second;

            rMember.intensity = std::max(rMember.intensity, newR.intensity);
            rMember.color.r += newR.color.r;
            rMember.color.g += newR.color.g;
            rMember.color.b += newR.color.b;

            rMember.colorAcc += newR.colorAcc;
            rMember.temperatureAcc += newR.temperatureAcc;
        }
    }

    std::vector<FixtureId> ids;
    ids.reserve(resolved.size());

    for (auto &f : resolved) {
        auto id      = f.first;
        auto &member = f.second;
        ids.push_back(id);

        auto fixture = fixtureManager.GetFixture(id);
        if (!fixture) {
            continue;
        }

        ColorRGB color = {member.color.r / member.colorAcc, member.color.g / member.colorAcc,
                          member.color.b / member.colorAcc};

        float temperature = member.temperature / member.colorAcc;

        fixture->SetIntensity(member.intensity);
        fixture->SetColor(color);
        //fixture->SetTemperature(temperature);
    }

    return ids;
}
