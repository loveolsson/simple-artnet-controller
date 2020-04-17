#pragma once
#include <OSC++/OSCUnpacker.h>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

template <typename T>
inline std::optional<T> OSCUnpackValue(OSCUnpacker &u);

template <>
inline std::optional<int32_t>
OSCUnpackValue(OSCUnpacker &u)
{
    int32_t i;
    if (u.unpackInt(&i)) {
        return i;
    }
    return std::nullopt;
}

template <>
inline std::optional<int64_t>
OSCUnpackValue(OSCUnpacker &u)
{
    int64_t i;
    if (u.unpackHyper(&i)) {
        return i;
    }
    return std::nullopt;
}

template <>
inline std::optional<float>
OSCUnpackValue(OSCUnpacker &u)
{
    float f;
    if (u.unpackFloat(&f)) {
        return f;
    }
    return std::nullopt;
}

template <>
inline std::optional<std::string>
OSCUnpackValue(OSCUnpacker &u)
{
    std::string str;
    if (u.unpackString(&str)) {
        return str;
    }
    return std::nullopt;
}

template <>
inline std::optional<std::vector<char>>
OSCUnpackValue(OSCUnpacker &u)
{
    std::string str;
    if (u.unpackBlob(&str)) {
        return std::vector<char>(str.begin(), str.end());
    }
    return std::nullopt;
}

template <typename T>
void OSCGetTypeName(std::ostringstream &str);

template <>
inline void
OSCGetTypeName<float>(std::ostringstream &str)
{
    str << "float ";
}

template <>
inline void
OSCGetTypeName<int32_t>(std::ostringstream &str)
{
    str << "int ";
}

template <>
inline void
OSCGetTypeName<int64_t>(std::ostringstream &str)
{
    str << "hyper ";
}

template <>
inline void
OSCGetTypeName<std::string>(std::ostringstream &str)
{
    str << "string ";
}

template <>
inline void
OSCGetTypeName<std::vector<char>>(std::ostringstream &str)
{
    str << "blob ";
}

template <typename T>
void OSCGetTypeIdentifier(std::ostringstream &str);

template <>
inline void
OSCGetTypeIdentifier<float>(std::ostringstream &str)
{
    str << "f";
}

template <>
inline void
OSCGetTypeIdentifier<int32_t>(std::ostringstream &str)
{
    str << "i";
}

template <>
inline void
OSCGetTypeIdentifier<int64_t>(std::ostringstream &str)
{
    str << "h";
}

template <>
inline void
OSCGetTypeIdentifier<std::string>(std::ostringstream &str)
{
    str << "s";
}

template <>
inline void
OSCGetTypeIdentifier<std::vector<char>>(std::ostringstream &str)
{
    str << "b";
}
