#ifndef _sock_h_
#define _sock_h_

#include <string>

class Sock
{
public:
    Sock(int connfd, int timeout = 240000);

    virtual ~Sock();

    int recvChunk(std::string& message);
    int sendChunk(const std::string& message);

    int flush();

protected:
    int    m_fd;
    char*  m_buffer;
    int    m_bufferSize;
    int    m_pointer;
    int    m_timeout;

private:
    Sock();
};

#endif
