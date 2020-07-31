#pragma once
#include "faders.hpp"
#include "fixturemanager.hpp"
#include "scene.hpp"
#include "settings.hpp"

#include <array>
#include <vector>

class SceneManager
{
    std::array<Scene, 8> scenes;

public:
    void LoadSettings(Settings &, FixtureManager &);
    void StoreSettings(Settings &);
    void ApplyChanges(FixtureManager &);
    void ScrubDeleatedFixtures(const std::vector<FixtureId> &deletions);
    std::vector<FixtureId> ResolveScenes(const std::array<FaderState, 8> &,
                                         FixtureManager &fixtureManager);

    Scene *GetScene(size_t index);
};

inline Scene *
SceneManager::GetScene(size_t index)
{
    if (index < this->scenes.size()) {
        return &this->scenes[index];
    }

    return nullptr;
}