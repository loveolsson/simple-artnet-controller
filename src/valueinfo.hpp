#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

struct ValueInfoOption {
  std::string name;
  std::string value;
};

struct ValueInfo {
  std::optional<int> numInt;
  std::optional<int> minInt;
  std::optional<int> maxInt;
  std::optional<int> stpInt;
  std::optional<double> numFloat;
  std::optional<double> minFloat;
  std::optional<double> maxFloat;
  std::optional<double> stpFloat;
  std::optional<std::string> str;
  std::optional<std::vector<ValueInfoOption>> opt;
};

struct ValueInfoListItem {
  std::string name;
  ValueInfo info;
};

struct Value {
  std::optional<int> numInt;
  std::optional<double> numFloat;
  std::optional<std::string> str;
};

struct ValueSetInfo {
  std::string name;
  Value value;
};

namespace nlohmann {
template <> struct adl_serializer<ValueInfoOption> {
  static void to_json(json &j, const ValueInfoOption &i) {
    j = json{
        {"name", i.name},
        {"value", i.value},
    };
  }
};

template <> struct adl_serializer<ValueInfo> {
  static void to_json(json &j, const ValueInfo &i) {
    j = json::object();

    if (i.numInt.has_value()) {
      j["numInt"] = *i.numInt;
    }

    if (i.minInt.has_value()) {
      j["minInt"] = *i.minInt;
    }

    if (i.maxInt.has_value()) {
      j["maxInt"] = *i.maxInt;
    }

    if (i.stpInt.has_value()) {
      j["stpInt"] = *i.stpInt;
    }

    if (i.numFloat.has_value()) {
      j["numFloat"] = *i.numFloat;
    }

    if (i.minFloat.has_value()) {
      j["minFloat"] = *i.minFloat;
    }

    if (i.maxFloat.has_value()) {
      j["maxFloat"] = *i.maxFloat;
    }

    if (i.stpFloat.has_value()) {
      j["maxFloat"] = *i.stpFloat;
    }

    if (i.str.has_value()) {
      j["str"] = *i.str;
    }

    if (i.opt.has_value()) {
      j["opt"] = *i.opt;
    }
  }
};

template <> struct adl_serializer<ValueInfoListItem> {
  static void to_json(json &j, const ValueInfoListItem &i) {
    j = json::object();
    j["name"] = i.name;
    j["info"] = i.info;
  }
};

template <> struct adl_serializer<Value> {
  static void from_json(const json &j, Value &i) {
    if (!j.is_object()) {
      return;
    }

    auto findInt = j.find("numInt");
    if (findInt != j.end() && findInt->is_number_integer()) {
      i.numInt = (*findInt).get<int>();
    }

    auto findFloat = j.find("numFloat");
    if (findFloat != j.end() && findFloat->is_number_float()) {
      i.numFloat = (*findFloat).get<float>();
    }

    auto findStr = j.find("str");
    if (findStr != j.end() && findStr->is_string()) {
      i.str = (*findStr).get<std::string>();
    }
  }
};

template <> struct adl_serializer<ValueSetInfo> {
  static void from_json(const json &j, ValueSetInfo &i) {
    if (!j.is_object()) {
      return;
    }

    auto findName = j.find("name");
    if (findName != j.end() && findName->is_string()) {
      i.name = (*findName).get<std::string>();
    }

    auto findValue = j.find("value");
    if (findValue != j.end() && findValue->is_object()) {
      i.value = (*findValue).get<Value>();
    }
  }
};
} // namespace nlohmann