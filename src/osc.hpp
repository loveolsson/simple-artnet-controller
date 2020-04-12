#pragma once
#include <OSC++/InetTransportManager.h>
#include <OSC++/InetUDPMaster.h>
#include <OSC++/OSCAssociativeNamespace.h>
#include <OSC++/OSCCallable.h>
#include <OSC++/OSCProcessor.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

template <typename T> inline std::optional<T> UnpackValue(OSCUnpacker &u);
template <> inline std::optional<int> UnpackValue(OSCUnpacker &u) {
  int i;
  if (u.unpackInt(&i)) {
    return i;
  }

  return std::nullopt;
}

template <> inline std::optional<float> UnpackValue(OSCUnpacker &u) {
  float f;
  if (u.unpackFloat(&f)) {
    return f;
  }

  return std::nullopt;
}

template <> inline std::optional<std::string> UnpackValue(OSCUnpacker &u) {
  std::string str;
  if (u.unpackString(&str)) {
    return str;
  }

  return std::nullopt;
}

template <>
inline std::optional<std::vector<char>> UnpackValue(OSCUnpacker &u) {
  std::string str;
  if (u.unpackBlob(&str)) {
    return std::vector<char>(str.begin(), str.end());
  }

  return std::nullopt;
}

template <typename Func, typename Tu, typename... Ts,
          typename std::enable_if<sizeof...(Ts) == 0, void *>::type = nullptr>
inline const bool UnpackAndCall(OSCUnpacker &u, Func fn, Tu tuple) {
  std::apply(fn, tuple);
  return true;
}

template <typename Func, typename Tu, typename T, typename... Ts>
inline bool UnpackAndCall(OSCUnpacker &u, Func fn, Tu tuple) {
  auto v = UnpackValue<T>(u);
  if (v.has_value()) {
    auto newTupl = std::tuple_cat(tuple, std::make_tuple(*v));
    return UnpackAndCall<Func, decltype(newTupl), Ts...>(u, fn, newTupl);
  }
  return false;
}

template <typename T>
struct OSCCallbackUnwrapper
    : public OSCCallbackUnwrapper<decltype(&T::operator())> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct OSCCallbackUnwrapper<ReturnType (ClassType::*)(Args...) const> {
  template <typename Func> static bool Call(OSCUnpacker &u, Func fn) {
    return UnpackAndCall<Func, std::tuple<>, Args...>(u, fn, std::make_tuple());
  }
};

template <typename Func> class OSCHandler : public OSCCallable {
  Func fn;

public:
  OSCHandler(Func fn) : fn(fn) {}
  virtual void call(const std::string &data, Transmit *const) final {
    OSCUnpacker u;
    u.setData(data);

    std::string path;
    if (!u.unpackString(&path)) {
      std::cout << "No path" << std::endl;
      return;
    }

    if (!u.skipString()) {
      std::cout << "No types" << std::endl;
      return;
    }

    typedef OSCCallbackUnwrapper<decltype(fn)> unwrapper;
    if (!unwrapper::template Call<Func>(u, fn)) {
      std::cout << "Invalid parameters to " << path << std::endl;
    }
  }
};

class OSC {
  std::vector<std::unique_ptr<OSCCallable>> handlers;

  InetTransportManager transMan;
  OSCAssociativeNamespace nspace;
  OSCProcessor processor;
  InetUDPMaster udpMaster;

public:
  OSC();
  ~OSC();

  void Poll();

  template <typename Func> void AddTarget(const std::string &path, Func fn) {
    auto handler = std::make_unique<OSCHandler<Func>>(fn);

    this->nspace.add(path, handler.get());
    this->handlers.push_back(std::move(handler));
  }
};
