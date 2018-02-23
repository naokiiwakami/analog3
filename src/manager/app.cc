#include "manager/app.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/tokenizer.hpp>
#include <iostream>

namespace analog3 {

const char* App::prompt = "analog3> ";

App::App() {
}

App::~App() {
}

void App::run() {
  std::vector<std::string> argv;
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
        typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;
        boost::escaped_list_separator<char> sep('\\', ' ', '"');
        std::string input = expansion;
        tokenizer tokens(input, sep);
        for (auto token : tokens) {
          argv.push_back(token);
        }
        quit = ProcessInput(argv);
      }
      free(expansion);
    }
    free(line);
  } while (!quit);
}

bool App::ProcessInput(const std::vector<std::string>& args) {
  for (auto arg : args) {
    std::cout << arg << std::endl;
  }
  return !args.empty() && args[0] == "quit";
}

}  // namespace analog3
