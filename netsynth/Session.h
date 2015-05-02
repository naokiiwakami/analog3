#ifndef _session_hxx_
#define _session_hxx_

#include "Runnable.h"

#include <map>
#include <list>

class FileModuleDriver;
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

    static bool initialize();

    void run();

protected:
    void describe(const connector::Request& request, connector::Reply* reply);
    void modifyAttribute(const connector::Request& request, connector::Reply* reply);
    void addSubComponent(const connector::Request& request, connector::Reply* reply);
    void removeSubComponent(const connector::Request& request, connector::Reply* reply);

    Sock* m_sock;

    static std::map<std::string, FileModuleDriver*> m_modules;
    static std::list<FileModuleDriver*> m_modulesList;
};

#endif
