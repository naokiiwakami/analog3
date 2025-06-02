#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include "server/errors.h"
#include "server/finder.h"
#include "server/server.h"

const char* version = "0.1";

static void initialize() {
  analog3::AppError::Initialize();
}

int main(int argc, char *argv[]) {
  initialize();

  log4cplus::BasicConfigurator config;
  config.configure();

  log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
  LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("analog3 synth version " << version << " started"));

  analog3::Server* server = new analog3::Server(12345);

  std::string dirname = "data";
  analog3::Finder finder(dirname);
  analog3::Status st = finder.Load(server);
  if (st != analog3::Status::OK) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Failed to load schema in dierctory " << dirname));
    return 2;
  }

  if (server->Initialize() != analog3::Status::OK) {
    delete server;
    return 2;
  }
  st = server->Launch();
  delete server;
  return st == analog3::Status::OK ? 0 : 4;
}
