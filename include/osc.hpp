#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class InetTransportManager;
class OSCAssociativeNamespace;
class OSCProcessor;
class InetUDPMaster;
class OSCCallable;

class OSC
{
    std::unique_ptr<InetTransportManager> transMan;
    std::unique_ptr<OSCAssociativeNamespace> nspace;
    std::unique_ptr<OSCProcessor> processor;
    std::unique_ptr<InetUDPMaster> udpMaster;

    std::unique_ptr<std::thread> thread;
    std::atomic_bool running{false};

    void Run();

public:
    OSC();
    ~OSC();

    void StartThread();
    void Stop();

    void AddTarget(const std::string &path, OSCCallable *);
};