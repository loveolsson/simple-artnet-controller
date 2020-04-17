#pragma once
#include "oscvalueunpack.hpp"

#include <OSC++/OSCCallable.h>
#include <OSC++/OSCUnpacker.h>

#include <optional>
#include <sstream>
#include <vector>

// OSCUnpackOrCall is called when template type list is depleted, and tuple is
// applied to function
template <typename Func, typename Tu, typename... Ts,
          typename std::enable_if<sizeof...(Ts) == 0, void *>::type = nullptr>
inline const bool
OSCUnpackOrCall(OSCUnpacker &u, Func fn, const Tu tuple)
{
    std::apply(fn, tuple);
    return true;
}

// OSCUnpackOrCall calls OSCUnpackValue for the first type in the template type
// list. On success it recurses OSCUnpackOrCall with T removed from the type
// list.
template <typename Func, typename Tu, typename T, typename... Ts>
inline bool
OSCUnpackOrCall(OSCUnpacker &u, Func fn, const Tu tuple)
{
    auto v = OSCUnpackValue<typename std::decay<T>::type>(u);
    if (v.has_value()) {
        auto t = std::tuple_cat(std::move(tuple), std::make_tuple(std::move(*v)));
        return OSCUnpackOrCall<Func, decltype(t), Ts...>(u, fn, std::move(t));
    }
    return false;
}

template <typename... Ts, typename std::enable_if<sizeof...(Ts) == 0, void *>::type = nullptr>
inline const void
OSCGetTypeNames(std::ostringstream &)
{
}

template <typename... Ts, typename std::enable_if<sizeof...(Ts) == 0, void *>::type = nullptr>
inline const void
OSCGetTypeIdentifiers(std::ostringstream &)
{
}

template <typename T, typename... Ts>
inline void
OSCGetTypeNames(std::ostringstream &str)
{
    OSCGetTypeName<typename std::decay<T>::type>(str);
    OSCGetTypeNames<Ts...>(str);
}

template <typename T, typename... Ts>
inline void
OSCGetTypeIdentifiers(std::ostringstream &str)
{
    OSCGetTypeIdentifier<typename std::decay<T>::type>(str);
    OSCGetTypeIdentifiers<Ts...>(str);
}

template <typename T>
struct OSCCallbackUnwrapper : public OSCCallbackUnwrapper<decltype(&T::operator())> {
};

template <typename ClassType, typename ReturnType, typename... Args>
struct OSCCallbackUnwrapper<ReturnType (ClassType::*)(Args...) const> {
    template <typename Func>
    static bool
    Call(OSCUnpacker &u, Func fn)
    {
        auto tuple = std::make_tuple();
        return OSCUnpackOrCall<Func, std::tuple<>, Args...>(u, fn, std::move(tuple));
    }

    static std::string
    GetCallSignatureString()
    {
        std::ostringstream res;
        res << "[ ";
        OSCGetTypeNames<Args...>(res);
        res << "]";
        return res.str();
    }

    static std::string
    GetCallIdentifierString()
    {
        std::ostringstream res;
        res << ",";
        OSCGetTypeIdentifiers<Args...>(res);
        return res.str();
    }
};

template <typename Func>
class OSCHandler : public OSCCallable
{
    Func fn;

public:
    OSCHandler(Func fn);
    virtual void call(const std::string &data, Transmit *const) final;
};

template <typename Func>
inline OSCHandler<Func>::OSCHandler(Func fn)
    : fn(fn)
{
}

template <typename Func>
inline void
OSCHandler<Func>::call(const std::string &data, Transmit *const)
{
    OSCUnpacker u;
    u.setData(data);

    std::string path, types;
    if (!u.unpackString(&path)) {
        std::cout << "No path" << std::endl;
        return;
    }

    if (!u.unpackString(&types)) {
        std::cout << "No types" << std::endl;
        return;
    }

    typedef OSCCallbackUnwrapper<decltype(fn)> unwrapper;
    auto expected = unwrapper::GetCallIdentifierString();

    if (expected == types) {
        if (!unwrapper::template Call<Func>(u, fn)) {
            std::cout << "Could not parse command " << path << ", with parameters "
                      << unwrapper::GetCallSignatureString() << std::endl;
        }
    } else {
        std::cout << "Invalid parameters to " << path << ", expected " << expected.substr(1)
                  << " got " << types.substr(1) << std::endl;
    }
}