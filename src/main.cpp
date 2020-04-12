#include "httpserver.hpp"
#include "httpstatic.hpp"
#include "osc.hpp"
#include "settings.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

extern "C" {
#include "artnet/artnet.h"
}

static volatile int keepRunning = 1;
static void intHandler(int) { keepRunning = 0; }

int main(int argc, char *argv[]) {
  signal(SIGINT, intHandler);

  if (argc != 2) {
    return 1;
  }

  OSC osc;
  osc.AddTarget("/test", [](int a, std::string b) {
    std::cout << "Call to /test with " << a << ", " << b << std::endl;
  });

  Settings settings;
  HTTPServer server;
  AssignStaticFiles(&server);

  auto node = artnet_new(argv[1], 0);
  if (!node) {
    printf("Unable to set up artnet node: %s\n", artnet_strerror());
    return 1;
  }

  artnet_start(node);

  server.Start();

  while (keepRunning) {
    osc.Poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  server.Stop();

  artnet_stop(node);

  return EXIT_SUCCESS;
}