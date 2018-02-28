#include "manager/app.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/tokenizer.hpp>
#include <iostream>
#include "protocol/synthserv.pb.h"

namespace analog3 {

const char* App::prompt = "analog3> ";

App::App() {
}

App::~App() {
}

void App::run() {
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
}

/**
 * @return true if you want to quit
 */
bool App::ProcessInput(const std::vector<std::string>& args) {
  const std::string& command = args[0];
  if (command == "quit") {
    return true;
  } else if (command == "listmodels") {
    std::cout << "listmodels" << std::endl;
    a3proto::Request request;
    request.set_op(a3proto::Request::LIST_MODELS);
  } else {
    std::cout << command << ": command not found" << std::endl;
  }
  return false;
}

}  // namespace analog3