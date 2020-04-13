#pragma once
#include "oschandler.hpp"

#include <OSC++/InetTransportManager.h>
#include <OSC++/InetUDPMaster.h>
#include <OSC++/OSCAssociativeNamespace.h>
#include <OSC++/OSCProcessor.h>

#include <memory>
#include <string>
#include <vector>

class OSC
{
    std::vector<std::unique_ptr<OSCCallable>> handlers;

    InetTransportManager transMan;
    OSCAssociativeNamespace nspace;
    OSCProcessor processor;
    InetUDPMaster udpMaster;

public:
    OSC();
    ~OSC();

    void Poll();

    template <typename Func>
    void
    AddTarget(const std::string &path, Func fn)
    {
        auto handler = std::make_unique<OSCHandler<Func>>(fn);

        this->nspace.add(path, handler.get());
        this->handlers.push_back(std::move(handler));
    }
};
