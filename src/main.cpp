#include "artnet.hpp"
#include "faders.hpp"
#include "fadershttp.hpp"
#include "fadersmidi.hpp"
#include "fadersosc.hpp"
#include "fixturemanager.hpp"
#include "httpserver.hpp"
#include "httpstatic.hpp"
#include "midi.hpp"
#include "osc.hpp"
#include "settings.hpp"

#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

static volatile int keepRunning = 1;
static void
intHandler(int)
{
    keepRunning = 0;
}

int
main(int argc, char *argv[])
{
    signal(SIGINT, intHandler);

    if (argc != 2) {
        return EXIT_FAILURE;
    }

    Settings settings;
    FixtureManager fixtureManager;

    fixtureManager.CreateFixture("Dimmer");
    fixtureManager.CreateFixture("Dimmer");
    fixtureManager.CreateFixture("Dimmer");

    HTTPServer httpServer;
    AssignStaticFiles(&httpServer);

    OSC osc;
    MIDI midi;
    Artnet artnet(argv[1]);

    FaderBank faderBank(settings);

    MIDIFaders midiFaders(midi, faderBank);
    OSCFaders oscFaders(osc, faderBank);
    HTTPFaders httpFaders(httpServer, faderBank);

    httpServer.Start();

    while (keepRunning) {
        fixtureManager.Poll();
        midi.Poll();
        osc.Poll();
        faderBank.Poll();

        artnet.Throttle();
        artnet.Send();
    }

    httpServer.Stop();

    return EXIT_SUCCESS;
}