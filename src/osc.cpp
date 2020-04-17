#include "osc.hpp"

#include <OSC++/InetTransportManager.h>
#include <OSC++/InetUDPMaster.h>
#include <OSC++/OSCAssociativeNamespace.h>
#include <OSC++/OSCProcessor.h>

#include <iostream>
#include <thread>

OSC::OSC()
{
    transMan  = std::make_unique<InetTransportManager>();
    nspace    = std::make_unique<OSCAssociativeNamespace>();
    processor = std::make_unique<OSCProcessor>();
    udpMaster = std::make_unique<InetUDPMaster>();

    this->processor->setNamespace(nspace.get());
    this->udpMaster->setProcessor(processor.get());
    this->udpMaster->setTransportManager(transMan.get());
}

OSC::~OSC()
{
    // Needed for unique destructors
}

void
OSC::Run()
{
    while (this->running) {
        this->transMan->runCycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void
OSC::StartThread()
{
    this->udpMaster->startlisten(10001);
    this->running = true;
    this->thread  = std::make_unique<std::thread>(std::bind(&OSC::Run, this));
}

void
OSC::Stop()
{
    this->running = false;
    if (this->thread && this->thread->joinable()) {
        this->thread->join();
        this->thread.reset();
    }

    this->udpMaster->stoplisten();
}

void
OSC::AddTarget(const std::string &path, OSCCallable *handler)
{
    this->nspace->add(path, handler);
}