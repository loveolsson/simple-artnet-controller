#include "artnet.hpp"
#include "faders.hpp"
#include "fadershttp.hpp"
#include "fadersmidi.hpp"
#include "fadersosc.hpp"
#include "fixturemanager.hpp"
#include "fixturemanagerhttp.hpp"
#include "httpserver.hpp"
#include "httpstatic.hpp"
#include "midi.hpp"
#include "osc.hpp"
#include "scenemanager.hpp"
#include "settings.hpp"

#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <string>

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
        std::cout << "Usage: ./simple-artnet 0.0.0.0 (the IP to send ArtNet on)" << std::endl;
        return EXIT_FAILURE;
    }

    Settings settings;
    settings.LoadSettings();

    FixtureManager fixtureManager;
    fixtureManager.Init(settings);

    SceneManager sceneManager;
    sceneManager.LoadSettings(settings, fixtureManager);

    FaderBank faderBank;
    faderBank.LoadSettings(settings);

    // fixtureManager.CreateFixture("Dimmer");
    // fixtureManager.CreateFixture("RGB");
    // fixtureManager.CreateFixture("RGBA");

    HTTPServer httpServer;
    InitStaticFiles(&httpServer);
    InitFixtureManagerHTTP(httpServer, fixtureManager);

    OSC osc;
    MIDI midi;
    Artnet artnet;

    InitMIDIFaders(midi, faderBank);
    InitOSCFaders(osc, faderBank);
    HTTPFaders httpFaders(httpServer, faderBank);

    httpServer.StartThread(8080);
    osc.StartThread();
    artnet.Start(argv[1]);

    while (keepRunning) {
        httpFaders.Poll();
        midi.PollEvents();

        const auto deletions = fixtureManager.ApplyChangesAndGetDeletions();
        if (!deletions.empty()) {
            sceneManager.ScrubDeleatedFixtures(deletions);
        }

        const auto faderState = faderBank.ApplyChangesAndGetState();

        sceneManager.ApplyChanges(fixtureManager);
        const auto activeIds = sceneManager.ResolveScenes(faderState, fixtureManager);

        fixtureManager.CalculateFixtureValues();

        artnet.SetUniversesZero();
        fixtureManager.WriteValuesToUniverses(artnet, activeIds);

        Artnet::WaitForNextFrame();
        artnet.SendDMX();
    }

    artnet.Stop();
    osc.Stop();
    httpServer.Stop();

    sceneManager.StoreSettings(settings);
    fixtureManager.StoreSettings(settings);
    faderBank.StoreSettings(settings);

    settings.SaveSettings();

    return EXIT_SUCCESS;
}