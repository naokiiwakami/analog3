#ifndef _session_hxx_
#define _session_hxx_

#include "Runnable.h"

#include <string>
#include <map>
#include <list>

class ModuleDriver;
class Sock;
namespace connector {
    class Request;
    class Reply;
}

class Session : public Runnable
{
public:
    Session(Sock* sock);
    virtual ~Session();

    static bool initialize(const char* rackURL);

    void run();

protected:
    void describe(const connector::Request& request, connector::Reply* reply);
    void modifyAttribute(const connector::Request& request, connector::Reply* reply);
    void addSubComponent(const connector::Request& request, connector::Reply* reply);
    void removeSubComponent(const connector::Request& request, connector::Reply* reply);

    Sock* m_sock;

    static std::map<std::string, ModuleDriver*> m_modules;
    static std::list<ModuleDriver*> m_modulesList;
};

#endif
