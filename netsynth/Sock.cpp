#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

#include "Sock.h"

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Sock"));

static const int DEFAULT_BUF_SIZE = 4096;

Sock::Sock(int connfd, int timeout) :
    m_fd(connfd),
    m_bufferSize(DEFAULT_BUF_SIZE),
    m_pointer(0),
    m_timeout(timeout)
{
    m_buffer = new char[m_bufferSize];

    // set blocking I/O
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags < 0) {
        // TODO: log
    }
    else if ((flags & O_NONBLOCK) != 0) {
        flags &= ~O_NONBLOCK;
        fcntl(m_fd, F_SETFL, flags);
    }
}

Sock::~Sock()
{
    delete[] m_buffer;
    close( m_fd );
}

int Sock::recvChunk(std::string& message)
{
    static const std::string fname = "Sock::recvChunk()";

    message.clear();

    // we go simple first using blocking i/o
    uint32_t netlong;
    int nread;
    while (true) {
        nread = read(m_fd, &netlong, sizeof(uint32_t));
        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": NioReadError: " << strerror(errno)));
                return nread;
            }
        }
        else if (nread == 0) { // EOF
            return -1;
        }
        break;
    }

    uint32_t size = ntohl(netlong);

    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": size=" << size));

    int remaining = size;
    while (remaining > 0) {
        int bytesToRead = (remaining < m_bufferSize) ? remaining : m_bufferSize;
        nread = read(m_fd, m_buffer, bytesToRead);
        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                return nread;
            }
        }
        else if (nread == 0) {
            return -1;
        }
        message.append(m_buffer, nread);
        remaining -= nread;
    }

    return 0;
}

int Sock::sendChunk(const std::string& message)
{
    static const std::string fname = "Sock::sendChunk";

    uint32_t netlong = htonl(message.size());
    int nwrite;
    int remaining = sizeof(netlong);
    while (remaining > 0) {
        nwrite = send(m_fd, &netlong + sizeof(netlong) - remaining, sizeof(netlong), 0);
        // nwrite = write(m_fd, &netlong + sizeof(netlong) - remaining, sizeof(netlong));
        if (nwrite < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": NioWriteError: " << strerror(errno)));
                return nwrite;
            }
        }
        remaining -= nwrite;
    }

    int size = message.size();
    remaining = size;
    while (remaining > 0) {
        nwrite = write(m_fd, message.c_str() + size - remaining, remaining);
        if (nwrite < 0) {
            if (errno == EINTR) {
                continue;
            }
            else {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": NioWriteError: " << strerror(errno)));
                return nwrite;
            }
        }
        remaining -= nwrite;
    }

    return 0;
}

int
Sock::flush()
{
    return fsync(m_fd);
}
