#include "osc.hpp"

#include <iostream>

OSC::OSC()
{
    this->processor.setNamespace(&nspace);
    this->udpMaster.setProcessor(&processor);
    this->udpMaster.setTransportManager(&transMan);
    this->udpMaster.startlisten(10001);
}

OSC::~OSC()
{
}

void
OSC::Poll()
{
    this->transMan.runCycle();
}
