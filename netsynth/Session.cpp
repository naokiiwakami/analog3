#include "Session.h"

#include "ModuleDriver.h"
#include "ModuleRecognitionException.h"
#include "Sock.h"
#include "connector.pb.h"

#include <iostream>
#include <sys/types.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

using namespace google::protobuf;

std::map<std::string, ModuleDriver*> Session::m_modules;
std::list<ModuleDriver*> Session::m_modulesList;

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Session"));

Session::Session(Sock* sock)
    : m_sock(sock)
{
}

Session::~Session()
{
    delete m_sock;
}

bool
Session::initialize(const char* rackURL)
{
    static const std::string fname = "Session::initialize()";

    RackDriver* rackDriver = RackDriver::create(rackURL);
    if (rackDriver == NULL) {
        return false;
    }

    if (!rackDriver->discover(&m_modulesList)) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": Error occurred during module discovery"));
        return false;
    }

    std::list<ModuleDriver*>::iterator it;
    for (it = m_modulesList.begin(); it != m_modulesList.end(); ++it) {
        ModuleDriver* moduleDriver = *it;
        const std::string& name = moduleDriver->getFullName();
        if (!name.empty()) {
            m_modules[name] = moduleDriver;
        }
        // TODO: also make module id table.
    }

    return true;
}

void
Session::run()
{
    static const std::string fname = "Session::run()";
    std::string message;
    while (true) {
        int result = m_sock->recvChunk(message);
        if (result < 0) {
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("terminating session (recv)"));
            return;
        }

        // Parse the request message and dispatch it
        connector::Request request;
        if (!request.ParseFromString(message)) {
            // Session would be out of sync if parse fails.
            // Thus drop this connection.
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Request parse error. Terminating connection."));
            return;
        }

        if (logger.getLogLevel() <= log4cplus::DEBUG_LOG_LEVEL) {
            std::string dump;
            io::StringOutputStream output(&dump);
            TextFormat::Print(request, &output);
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": request=" << dump));
        }

        connector::Reply reply;

        switch (request.command()) {
        case connector::Request::DESCRIBE:
            describe(request, &reply);
            break;
        case connector::Request::SET_ATTRIBUTE:
        case connector::Request::UNSET_ATTRIBUTE:
            modifyAttribute(request, &reply);
            break;
        case connector::Request::ADD_SUBCOMPONENT:
            addSubComponent(request, &reply);
            break;
        case connector::Request::REMOVE_SUBCOMPONENT:
            removeSubComponent(request, &reply);
            break;
        }

        std::string replyMessage;
        if (!reply.SerializeToString(&replyMessage)) {
            return;
        }
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": size of serialized reply=" << replyMessage.size()));

        result = m_sock->sendChunk(replyMessage);
        if (result < 0) {
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("terminating session (send)"));
            return;
        }
    }
}

void
Session::describe(const connector::Request& request, connector::Reply* reply)
{
    static const std::string fname = "Session::describe()";
    std::list<ModuleDriver*>::iterator it = m_modulesList.begin();
    std::list<ModuleDriver*>::iterator end = m_modulesList.end();

    for (; it != end; ++it) {
        ModuleDriver* moduleDriver = *it;
        std::string errorMessage;
        if (!moduleDriver->describe(reply->add_component(), &errorMessage)) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << errorMessage));
            reply->clear_component();
            reply->set_status(connector::Reply::ERROR);
            return;
        }

        if (logger.getLogLevel() <= log4cplus::DEBUG_LOG_LEVEL) {
            std::string dump;
            io::StringOutputStream output(&dump);
            TextFormat::Print(*reply, &output);
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": reply=" << dump));
        }
    }

    reply->set_status(connector::Reply::SUCCESS);
}

void
Session::modifyAttribute(const connector::Request& request, connector::Reply* reply)
{
    static const std::string fname = "Session::modifyAttribute()";
    
    struct Error {
        Error(const std::string& message) {
            this->message = message;
        }
        std::string message;
    };

    try {
        // Read the request.
        if (!request.has_attribute()) {
            throw Error("Invalid request: attribute modifier is missing");
        }

        // Read target path.
        int pathLevels = request.path_size();
        if (pathLevels < 1) {
            throw Error("Invalid request: target path is incomplete");
        }
        const std::string& moduleName = request.path(0);
        std::map<std::string, ModuleDriver*>::iterator it = m_modules.find(moduleName);
        if (it == m_modules.end()) {
            std::string message = moduleName;
            message += ": target module not found.";
            throw Error(message);
        }

        // Execute modification
        std::string errorMessage;
        if (!it->second->modifyAttribute(request, &errorMessage)) {
            throw Error(errorMessage);
        }
        
        reply->set_status(connector::Reply::SUCCESS);
    }
    catch(Error& ex) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << ex.message));
        reply->set_status(connector::Reply::ERROR);
        reply->set_message(ex.message);
    }
}

void
Session::addSubComponent(const connector::Request& request, connector::Reply* reply)
{
    static const std::string fname = "Session::addSubComponent()";

    struct Error {
        Error(const std::string& message) {
            this->message = message;
        }
        std::string message;
    };

    try {
        // Read the request.
        if (!request.has_component()) {
            throw Error("Invalid request: attribute modifier is missing");
        }
        const std::string& moduleName = request.path(0);
        std::map<std::string, ModuleDriver*>::iterator it = m_modules.find(moduleName);
        if (it == m_modules.end()) {
            std::string message = moduleName;
            message += ": target module not found.";
            throw Error(message);
        }

        // Executtion
        std::string errorMessage;
        if (!it->second->addSubComponent(request, &errorMessage)) {
            throw Error(errorMessage);
        }
        reply->set_status(connector::Reply::SUCCESS);
    }
    catch(Error& ex) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << ex.message));
        reply->set_status(connector::Reply::ERROR);
    }
}

void
Session::removeSubComponent(const connector::Request& request, connector::Reply* reply)
{
    static const std::string fname = "Session::removeSubComponent()";

    struct Error {
        Error(const std::string& message) {
            this->message = message;
        }
        std::string message;
    };

    try {
        // Read the request.
        if (!request.has_component()) {
            throw Error("Invalid request: attribute modifier is missing");
        }

        const std::string& moduleName = request.path(0);
        std::map<std::string, ModuleDriver*>::iterator it = m_modules.find(moduleName);
        if (it == m_modules.end()) {
            std::string message = moduleName;
            message += ": target module not found.";
            throw Error(message);
        }

        // Executtion
        std::string errorMessage;
        if (!it->second->removeSubComponent(request, &errorMessage)) {
            throw Error(errorMessage);
        }
        reply->set_status(connector::Reply::SUCCESS);
    }
    catch(Error& ex) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << ex.message));
        reply->set_status(connector::Reply::ERROR);
    }
}
