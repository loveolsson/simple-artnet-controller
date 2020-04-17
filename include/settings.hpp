#pragma once
#include <nlohmann/json_fwd.hpp>

class Settings
{
private:
    nlohmann::json data;

public:
    void LoadSettings();
    void SaveSettings();

    int GetInt(const std::string &name, int d = 0) const;
    void SetInt(const std::string &name, int val);

    float GetFloat(const std::string &name, float d = 0) const;
    void SetFloat(const std::string &name, float val);

    std::string GetString(const std::string &name, const std::string &d = "") const;
    void SetString(const std::string &name, const std::string &val);

    nlohmann::json GetJSON(const std::string &name, const nlohmann::json &d) const;
    void SetJSON(const std::string &name, const nlohmann::json &val);
};
