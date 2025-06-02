#include <getopt.h>
#include <iostream>

#include "manager/app.h"

int main(int argc, char* argv[]) {
  bool is_stub = false;
  std::string host = "localhost";
  int port = 12345;
  while (true) {
    static struct option long_options[] = {
      { "stub", 0, nullptr, 0 },
      { "host", 1, nullptr, 'h' },
      { "port", 1, nullptr, 'p' },
      { 0, 0, 0, 0 },
    };
    int option_index = 0;
    int c = getopt_long(argc, argv, "h:p:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            is_stub = true;
            break;
          case 1:
            host = optarg;
            break;
          case 2:
            port = atoi(optarg);
            break;
        }
        break;
      case 'h':
        host = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        break;
    }
  }

  analog3::App* app = new analog3::App(is_stub, host, port);
  int status = app->run();
  delete app;
  return status;
}
