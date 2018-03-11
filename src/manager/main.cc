#include <getopt.h>
#include <iostream>

#include "manager/app.h"

int main(int argc, char* argv[]) {
  bool is_stub = false;
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
      { "stub", 0, nullptr, 0 },
      { 0, 0, 0, 0 },
    };
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            is_stub = true;
            break;
        }
        break;
    }
  }

  analog3::App* app = new analog3::App(is_stub);
  int status = app->run();
  delete app;
  return status;
}
