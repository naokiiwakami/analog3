#include "manager/app.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/tokenizer.hpp>
#include <iostream>
#include "api/synthserv.pb.h"

namespace analog3 {

const char* App::prompt = "analog3> ";

App::App(bool is_stub) {
  if (is_stub) {
    _service = new StubSynthService();
  } else {
    _service = new NetSynthService("localhost", 12345);
  }
}

App::~App() {
  delete _service;
}

int App::run() {
  int status;
  if ((status = _service->Plugin()) != 0) {
    return status;
  }
  std::vector<std::string> argv;
  boost::escaped_list_separator<char> sep("", " ", "\"\'");
  bool quit = false;
  do {
    char* line = readline(prompt);
    if (line == NULL) {
      break;
    }

    if (*line != '\0') {
      char* expansion;
      int result = history_expand(line, &expansion);

      if (result < 0 || result == 2) {
        printf("%s\n", expansion);
      } else {
        add_history(expansion);
        argv.clear();
        std::string input = expansion;
        boost::tokenizer<boost::escaped_list_separator<char>> tokens(input, sep);
        for (auto token : tokens) {
          if (!token.empty()) {
            argv.push_back(token);
          }
        }
        if (!argv.empty()) {
          quit = ProcessInput(argv);
        }
      }
      free(expansion);
    }
    free(line);
  } while (!quit);

  return _service->Plugout();
}

/**
 * @return true if you want to quit
 */
bool App::ProcessInput(const std::vector<std::string>& args) {
  const std::string& command = args[0];
  int result;
  if (command == "quit") {
    return true;
  } else if (command == "ping") {
    _service->Ping();
    std::cout << "PONG" << std::endl;
  } else if (command == "listmodelids") {
    std::vector<uint16_t> model_ids;
    result = _service->ListModelIds(&model_ids);
    if (result == 0) {
      for (uint32_t id : model_ids) {
        std::cout << id << std::endl;
      }
    } else {
      std::cout << "ERROR: " << result << std::endl;
    }
  } else if (command == "getmodels") {
    int value;
  } else {
    std::cout << command << ": command not found" << std::endl;
  }
  return false;
}

}  // namespace analog3
