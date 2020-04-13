#include "fixturemanager.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <streambuf>
#include <string>

using namespace nlohmann;

static const char *fixtureFile = "fixtures.json";

FixtureId static GetRandomId()
{
    static std::random_device rd;
    static std::mt19937_64 e2(rd());

    return e2();
}

FixtureManager::FixtureManager()
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
}

FixtureId
FixtureManager::FindFreeId()
{
    auto test = GetRandomId();
    while (this->GetFixture(test)) {
        test = GetRandomId();
    }

    return test;
}

std::shared_ptr<Fixture>
FixtureManager::GetFixture(FixtureId id)
{
    auto find = std::find_if(this->fixtures.begin(), this->fixtures.end(),
                             [id](const auto &fixture) { return fixture->id == id; });

    if (find != this->fixtures.end()) {
        return *find;
    }
    return nullptr;
}

void
FixtureManager::CreateFixtureInternal(const std::string &type)
{
    auto desc = this->fixtureDescs.find(type);
    if (desc == this->fixtureDescs.end()) {
        return;
    }

    auto id = this->FindFreeId();

    auto fixture = std::make_shared<Fixture>(id, *desc, nullptr);
    this->fixtures.push_back(fixture);
}

void
FixtureManager::CreateFixture(const std::string &type)
{
    ArrStr str;

    if (type.length() > str.size() - 1) {
        return;
    }

    type.copy(str.data(), str.size() - 1);
    this->createQueue.enqueue(str);
}

void
FixtureManager::Poll()
{
    ArrStr str;
    while (this->createQueue.try_dequeue(str)) {
        this->CreateFixtureInternal(str.data());
    }
}