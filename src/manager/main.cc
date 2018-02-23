#include <iostream>

#include "manager/app.h"

int main(int argc, char* argv[]) {
  analog3::App* app = new analog3::App();
  app->run();
  return 0;
}
