#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "Listener.h"
#include "Session.h"
#include "connector.pb.h"

const char* version = "0.1";

Listener *listener;

bool initialize()
{
    if (!Session::initialize()) {
        return false;
    }
    return true;
}

int main ( int argc, char* argv[] )
{
    // To prevent process from being dropped by SIG_PIPE
    sigset(SIGPIPE, SIG_IGN);
 
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    log4cplus::BasicConfigurator config;
    config.configure();

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("netsynth version " << version << " started"));
    
    int port = 12345;
    if (argc > 1) {
        port = atoi( argv[1] );
        if (port <= 0 || port >= 65536) {
            LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT(argv[0] << ": Invalid port number: " << argv[1]));
            return 1;
        }
    }

    if (!initialize()) {
        LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT("Server initialization failed."));
        return 1;
    }
    
    // Launch the service listener
    listener = new Listener(port, NULL /* new SynthSessionFactory() */);
    if (!listener->initialize()) {
        LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT("Failed to start listening"));
    }
	
    // Launch the listener
    if (listener->acceptLoop()) {
        LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT("Could not launch the listener."));
        return 1;
    }

    // Waiting for shutdown of listener thread
    listener->waitForShutdown();

    return 0;
}
