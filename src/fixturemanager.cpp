#include "fixturemanager.hpp"

#include "typesjson.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <streambuf>
#include <string>

using namespace nlohmann;

static const char *fixtureFile = "fixtures.json";

static FixtureId
GetRandomId()
{
    static std::random_device rd;
    static std::mt19937_64 e2(rd());

    return e2();
}

void
FixtureManager::Init(Settings &settings)
{
    this->LoadFixtureDescs();
    this->LoadFixtureConf(settings);
    this->CacheConfig();
}

Fixture *
FixtureManager::GetFixture(const std::string &name, bool onlyActive)
{
    auto find =
        std::find_if(this->fixtures.begin(), this->fixtures.end(),
                     [&name](const auto &fixture) { return fixture.second.Name() == name; });

    if (find != this->fixtures.end()) {
        return &find->second;
    }

    return nullptr;
}

void
FixtureManager::LoadFixtureDescs()
{
    using itr = std::istreambuf_iterator<char>;
    std::ifstream t(fixtureFile);

    if (!t.is_open()) {
        throw std::invalid_argument("Could not open fixture file.");
    }

    std::string fileContents((itr(t)), itr());
    auto j = json::parse(fileContents);

    if (!j.is_object()) {
        throw std::invalid_argument("Fixture file JSON is not an object");
    }

    for (auto i = j.begin(); i != j.end(); i++) {
        this->fixtureDescs[i.key()] = i.value();
    }

    this->jsonFixtureDescs.SetData(std::move(j));
}

void
FixtureManager::LoadFixtureConf(Settings &settings)
{
    auto j = settings.GetJSON("fixtureManager", nullptr);
    if (j.is_null()) {
        return;
    }

    auto f = j["fixtures"];

    for (auto i = f.begin(); i != f.end(); i++) {
        FixtureId id     = std::stoull(i.key());
        std::string type = i.value()["type"];
        auto conf        = i.value()["config"];
        auto &desc       = this->fixtureDescs[type];

        this->fixtures.insert(std::make_pair(id, Fixture(type, id, desc, conf)));
    }
}

void
FixtureManager::StoreSettings(Settings &settings)
{
    auto j  = json::object();
    auto &f = j["fixtures"] = json::object();

    for (auto &fixture : this->fixtures) {
        f[std::to_string(fixture.first)] = json{
            {"type", fixture.second.Type()},
            {"config", fixture.second.SerializeConfig()},
        };
    }

    settings.SetJSON("fixtureManager", j);
}

FixtureId
FixtureManager::FindFreeId()
{
    FixtureId test;

    do {
        // Generate random ID until we get one that is not in use or 0, in case the random number generator is bad
        test = GetRandomId();
    } while (this->GetFixture(test, false) || test == 0);

    return test;
}

FixtureId
FixtureManager::CreateFixtureInternal(const std::string &type)
{
    auto desc = this->fixtureDescs.find(type);
    if (desc == this->fixtureDescs.end()) {
        std::cerr << "Trying to create uknown fixture type: " << type << std::endl;

        return 0;
    }

    auto id = this->FindFreeId();

    auto pair = std::make_pair(id, Fixture(type, id, desc->second, nullptr));

    // Find first free name in the most stupid way possible
    int i = 1;
    std::string name;
    do {
        name = type + " " + std::to_string(i++);
    } while (this->GetFixture(name, false));

    pair.second.Config(json{{"name", name}});

    this->fixtures.insert(pair);

    return id;
}

void
FixtureManager::CreateFixture(const std::string &type, std::optional<Location> loc)
{
    Instruction instruction{};
    instruction.createFixture = true;

    type.copy(instruction.str.data(), instruction.str.size() - 1);

    if (loc.has_value()) {
        instruction.setLocation = true;
        instruction.x           = loc->x;
        instruction.y           = loc->y;
    }

    this->instructionQueue.enqueue(instruction);
}

void
FixtureManager::DeleteFixture(FixtureId id)
{
    Instruction instruction{};
    instruction.deleteFixture = true;
    instruction.id            = id;

    this->instructionQueue.enqueue(instruction);
}

void
FixtureManager::SetName(FixtureId id, const std::string &name)
{
    Instruction instruction{};
    instruction.setName = true;
    instruction.id      = id;
    name.copy(instruction.str.data(), instruction.str.size() - 1);

    this->instructionQueue.enqueue(instruction);
}

void
FixtureManager::SetUniverse(FixtureId id, int v)
{
    Instruction instruction{};
    instruction.setUniverse = true;
    instruction.id          = id;
    instruction.x           = v;

    this->instructionQueue.enqueue(instruction);
}

void
FixtureManager::SetChannel(FixtureId id, int v)
{
    Instruction instruction{};
    instruction.setChannel = true;
    instruction.id         = id;
    instruction.x          = v;

    this->instructionQueue.enqueue(instruction);
}

void
FixtureManager::SetLocation(FixtureId id, Location v)
{
    Instruction instruction{};
    instruction.setLocation = true;
    instruction.id          = id;
    instruction.x           = v.x;
    instruction.y           = v.y;

    this->instructionQueue.enqueue(instruction);
}

std::vector<FixtureId>
FixtureManager::ApplyChangesAndGetDeletions()
{
    std::vector<FixtureId> deletions;

    Instruction instruction;
    bool dirty = false;

    while (this->instructionQueue.try_dequeue(instruction)) {
        dirty = true;

        if (instruction.createFixture) {
            instruction.id = this->CreateFixtureInternal(instruction.str.data());
        }

        auto fixture = this->GetFixture(instruction.id, false);

        if (!fixture) {
            continue;
        }

        if (instruction.setLocation) {
            fixture->Config(json{{"location", Location{instruction.x, instruction.y}}});
        } else if (instruction.deleteFixture) {
            this->fixtures.erase(instruction.id);
            deletions.push_back(instruction.id);
        } else if (instruction.setName) {
            fixture->Config(json{{"name", instruction.str.data()}});
        } else if (instruction.setUniverse) {
            fixture->Config(json{{"universe", instruction.x}});
        } else if (instruction.setChannel) {
            fixture->Config(json{{"channel", instruction.x}});
        }
    }

    if (dirty) {
        this->CacheConfig();
    }

    this->jsonConfigCache.Poll();

    return deletions;
}

void
FixtureManager::CalculateFixtureValues()
{
    for (auto &fixture : this->fixtures) {
        if (fixture.second.Active()) {
            fixture.second.Calculate();
        }
    }
}

void
FixtureManager::WriteValuesToUniverses(Artnet &artnet, const std::vector<FixtureId> &ids)
{
    for (auto &id : ids) {
        auto fixture = this->GetFixture(id);
        if (fixture) {
            fixture->WriteValuesToUniverse(artnet);
        }
    }
}

void
FixtureManager::CacheConfig()
{
    auto j = json::object();
    for (auto &fixture : this->fixtures) {
        j[std::to_string(fixture.first)] = std::move(fixture.second.SerializeConfig());
    }

    this->jsonConfigCache.SetData(std::move(j));
}