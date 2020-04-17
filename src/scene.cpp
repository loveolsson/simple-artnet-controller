#include "scene.hpp"

using namespace nlohmann;

SceneMember *
Scene::GetMember(FixtureId id)
{
    auto find = this->members.find(id);
    if (find != this->members.end()) {
        return &find->second;
    }

    return nullptr;
}

SceneMember *
Scene::GetOrCreateMember(FixtureId id)
{
    auto res = this->GetMember(id);

    if (!res) {
        this->members.insert(std::make_pair(id, SceneMember{}));
        res = this->GetMember(id);

        assert(res);
    }

    return res;
}

void
Scene::RemoveFixture(FixtureId id)
{
    this->members.erase(id);
}

void
Scene::SetIntensity(FixtureId id, IntVal v)
{
    Instruction instruction{};
    instruction.setIntensity = true;
    instruction.id           = id;
    instruction.intVal       = v;

    this->instructionQueue.enqueue(instruction);
}

void
Scene::SetTemperature(FixtureId id, IntVal v)
{
    Instruction instruction{};
    instruction.setTemperature = true;
    instruction.id             = id;
    instruction.intVal         = v;

    this->instructionQueue.enqueue(instruction);
}

void
Scene::SetColorIndex(FixtureId id, int16_t v)
{
    Instruction instruction{};
    instruction.setTemperature = true;
    instruction.id             = id;
    instruction.intVal         = v;

    this->instructionQueue.enqueue(instruction);
}

void
Scene::SetName(FixtureId id, const std::string &name)
{
    Instruction instruction{};
    instruction.setName = true;
    instruction.id      = id;
    name.copy(instruction.str.data(), instruction.str.size() - 1);

    this->instructionQueue.enqueue(instruction);
}

const std::map<FixtureId, SceneMember> &
Scene::GetMembers()
{
    return this->members;
}

void
Scene::LoadSettings(Settings &settings, FixtureManager &fixtureManager, int index)
{
    this->CacheConfig();
    auto s = settings.GetJSON("scene" + std::to_string(index), nullptr);
    if (s.is_null()) {
        this->name = "Scene " + std::to_string(index);
        return;
    }

    this->name       = s["name"];
    auto tempMembers = s["members"].get<std::map<std::string, SceneMember>>();

    this->members.clear();
    for (auto &m : tempMembers) {
        this->members.insert(std::make_pair(std::stoull(m.first), m.second));
    }
}

void
Scene::SaveSettings(Settings &settings, int index)
{
    std::map<std::string, SceneMember> tempMembers;
    for (auto &m : this->members) {
        tempMembers.insert(std::make_pair(std::to_string(m.first), m.second));
    }

    auto j = json{
        {"name", this->name},
        {"members", tempMembers},
    };

    settings.SetJSON("scene" + std::to_string(index), j);
}

void
Scene::Poll(FixtureManager &)
{
    Instruction instruction;
    bool dirty = false;

    while (this->instructionQueue.try_dequeue(instruction)) {
        dirty = true;

        if (instruction.setName) {
            this->name = instruction.str.data();
            continue;
        }

        auto member = this->GetOrCreateMember(instruction.id);

        if (instruction.setIntensity) {
            member->intensity = instruction.intVal;
        } else if (instruction.setTemperature) {
            member->temperature = instruction.intVal;
        } else if (instruction.setColorIndex) {
            member->colorIndex = instruction.intVal.val;
        }

        if (!member->Active()) {
            this->RemoveFixture(instruction.id);
        }
    }

    if (dirty) {
        this->CacheConfig();
    }
}

void
Scene::CacheConfig()
{
    // Will be dumping config to mutex locked cache
}
