#include "common.h"
#include <mbed_retarget.h>
#include <net_socket.h>
#include <rtos/EventFlags.h>

using namespace mbed;
using namespace rtos;

#define SOCKET_NOT_INITIALIZED (-1)
#define NO_FREE_SOCKET_SLOT (-1)

static BSDSocket sockets[MBED_NET_SOCKET_MAX_NUMBER];

BSDSocket::BSDSocket()
{
    fd       = SOCKET_NOT_INITIALIZED;
    callback = nullptr;
    flags    = 0;
}

BSDSocket::~BSDSocket()
{
    close();
}

int BSDSocket::open(int type)
{
    InternetSocket * socket;
    switch (type)
    {
    case MBED_TCP_SOCKET: {
        socket = new (&tcpSocket) TCPSocket();
    }
    break;
    case MBED_UDP_SOCKET: {
        socket = new (&udpSocket) UDPSocket();
    }
    break;
    default:
        set_errno(ESOCKTNOSUPPORT);
        return fd;
    };

    if (socket->open(NetworkInterface::get_default_instance()) != NSAPI_ERROR_OK)
    {
        close();
        set_errno(ENOBUFS);
        return fd;
    }

    this->type = type;

    fd = bind_to_fd(this);
    if (fd < 0)
    {
        close();
        set_errno(ENFILE);
        return fd;
    }

    flags = 0;
    sigio([&]() {
        flags = SOCKET_SIGIO_RX | SOCKET_SIGIO_TX;
        if (callback)
        {
            callback();
        }
    });

    return fd;
}

int BSDSocket::close()
{
    switch (type)
    {
    case MBED_TCP_SOCKET:
        tcpSocket.~TCPSocket();
        break;
    case MBED_UDP_SOCKET:
        udpSocket.~UDPSocket();
        break;
    }

    fd       = SOCKET_NOT_INITIALIZED;
    callback = nullptr;
    flags    = 0;
}

ssize_t BSDSocket::read(void *, size_t)
{
    flags &= ~SOCKET_SIGIO_RX;
    return 1;
}

ssize_t BSDSocket::write(const void *, size_t)
{
    flags &= ~SOCKET_SIGIO_TX;
    return 1;
}

off_t BSDSocket::seek(off_t offset, int whence)
{
    return -ESPIPE;
}

int BSDSocket::set_blocking(bool blocking)
{
    if (blocking)
    {
        return -EINVAL;
    }
    return 0;
}

bool BSDSocket::is_blocking() const
{
    return false;
}

short BSDSocket::poll(short events) const
{
    short ret      = 0;
    uint32_t state = flags;

    if ((events & POLLIN) && (state & SOCKET_SIGIO_RX))
    {
        ret |= POLLIN;
    }

    if ((events & POLLOUT) && (state & SOCKET_SIGIO_TX))
    {
        ret |= POLLOUT;
    }

    return ret;
}

void BSDSocket::sigio(Callback<void()> func)
{
    callback = func;
    if (callback && flags)
    {
        callback();
    }
}

Socket * BSDSocket::getNetSocket()
{
    Socket * ret = nullptr;
    switch (type)
    {
    case MBED_TCP_SOCKET:
        ret = &tcpSocket;
        break;
    case MBED_UDP_SOCKET:
        ret = &udpSocket;
        break;
    }
    return ret;
}

nsapi_version_t Inet2Nsapi(int family)
{
    switch (family)
    {
    case AF_INET:
        return NSAPI_IPv4;
        break;
    case AF_INET6:
        return NSAPI_IPv6;
        break;
    default:
        return NSAPI_UNSPEC;
    }
}

void msghdr2Netsocket(SocketAddress * dst, struct sockaddr_in * src)
{
    dst->set_ip_bytes(src->sin_addr.s4_addr, Inet2Nsapi(src->sin_family));
}

void Sockaddr2Netsocket(SocketAddress * dst, struct sockaddr * src)
{
    sockaddr_in * addr = reinterpret_cast<sockaddr_in *>(src);
    dst->set_ip_bytes(addr->sin_addr.s4_addr, Inet2Nsapi(src->sa_family));
}

static Socket * getSocket(int fd)
{
    BSDSocket * socket = static_cast<BSDSocket *>(mbed_file_handle(fd));

    if (socket == nullptr)
    {
        return nullptr;
    }

    return socket->getNetSocket();
}

int getFreeSocketSlotIndex()
{
    int index = NO_FREE_SOCKET_SLOT;
    for (int i = 0; i < MBED_NET_SOCKET_MAX_NUMBER; i++)
    {
        if (sockets[i].fd == SOCKET_NOT_INITIALIZED)
        {
            index = i;
            break;
        }
    }
    return index;
}

int mbed_socket(int family, int type, int proto)
{
    int fd    = -1;
    int index = getFreeSocketSlotIndex();
    if (index == NO_FREE_SOCKET_SLOT)
    {
        set_errno(ENOBUFS);
        return -1;
    }

    BSDSocket * socket = &sockets[index];
    return socket->open(type);
}

int mbed_socketpair(int family, int type, int proto, int sv[2])
{
    return 0;
}

int mbed_shutdown(int fd, int how)
{
    return 0;
}

int mbed_bind(int fd, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) addr);

    return socket->bind(sockAddr);
}

int mbed_connect(int fd, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) addr);

    return socket->connect(sockAddr);
}

int mbed_listen(int fd, int backlog)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }

    return socket->listen(backlog);
}

int mbed_accept(int fd, struct sockaddr * addr, socklen_t * addrlen)
{
    int retFd     = -1;
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }

    retFd = mbed_socket(AF_INET, BSDSocket::MBED_TCP_SOCKET, 0);
    if (retFd < 0)
    {
        return retFd;
    }

    return retFd;
}

ssize_t mbed_send(int fd, const void * buf, size_t len, int flags)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    return socket->send(buf, len);
}

ssize_t mbed_recv(int fd, void * buf, size_t max_len, int flags)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    return socket->recv(buf, max_len);
}

ssize_t mbed_sendto(int fd, const void * buf, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) dest_addr);

    return socket->sendto(sockAddr, buf, len);
}

ssize_t mbed_sendmsg(int fd, const struct msghdr * message, int flags)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;

    msghdr2Netsocket(&sockAddr, (struct sockaddr_in *) message->msg_name);

    return socket->sendto(sockAddr, (void *) message, sizeof(msghdr));
}

ssize_t mbed_recvfrom(int fd, void * buf, size_t max_len, int flags, struct sockaddr * src_addr, socklen_t * addrlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, src_addr);

    return socket->recvfrom(&sockAddr, buf, max_len);
}

int mbed_getsockopt(int fd, int level, int optname, void * optval, socklen_t * optlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    return socket->getsockopt(level, optname, optval, optlen);
}

int mbed_setsockopt(int fd, int level, int optname, const void * optval, socklen_t optlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    return socket->setsockopt(level, optname, optval, optlen);
}

int mbed_getsockname(int fd, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

int mbed_getpeername(int sockfd, struct sockaddr * addr, socklen_t * addrlen)
{
    auto * socket = getSocket(sockfd);
    if (socket == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, addr);
    return socket->getpeername(&sockAddr);
}

ssize_t mbed_recvmsg(int socket, struct msghdr * message, int flags)
{
    auto * fd = getSocket(socket);
    if (fd == nullptr)
    {
        set_errno(ENOBUFS);
        return -1;
    }
    SocketAddress sockAddr;
    msghdr2Netsocket(&sockAddr, (struct sockaddr_in *) message->msg_name);

    return fd->sendto(sockAddr, (void *) message, sizeof(msghdr));
}

int mbed_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout)
{
    int totalReady = 0;
    FileHandle * fh;
    short fdEvents    = POLLIN | POLLOUT | POLLERR;
    uint32_t waitTime = osWaitForever;
    uint32_t ret;

    EventFlags event;
    uint32_t eventFlag = 1;

    if (nfds < 0)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (timeout)
    {
        waitTime = (timeout->tv_sec * (uint32_t) 1000) + (timeout->tv_usec / (uint32_t) 1000);
    }

    for (int fd = 0; fd < nfds; ++fd)
    {
        fh = mbed_file_handle(fd);
        if (fh)
        {
            if (fh->poll(fdEvents))
            {
                event.set(eventFlag);
                break;
            }
            else
            {
                fh->sigio([&event, eventFlag]() { event.set(eventFlag); });
            }
        }
    }

    ret = event.wait_any(eventFlag, waitTime);
    if (ret & osFlagsError)
    {
        set_errno(EINTR);
        return -1;
    }

    for (int fd = 0; fd < nfds; ++fd)
    {
        fh = mbed_file_handle(fd);
        if (fh)
        {
            fdEvents = fh->poll(POLLIN | POLLOUT | POLLERR);

            if (readfds)
            {
                if (fdEvents & POLLIN)
                {
                    FD_SET(fd, readfds);
                    totalReady++;
                }
                else
                {
                    FD_CLR(fd, readfds);
                }
            }

            if (writefds)
            {
                if (fdEvents & POLLOUT)
                {
                    FD_SET(fd, writefds);
                    totalReady++;
                }
                else
                {
                    FD_CLR(fd, writefds);
                }
            }

            if (exceptfds)
            {
                if (fdEvents & POLLERR)
                {
                    FD_SET(fd, exceptfds);
                    totalReady++;
                }
                else
                {
                    FD_CLR(fd, exceptfds);
                }
            }
        }
    }

    for (int fd = 0; fd < nfds; ++fd)
    {
        fh = mbed_file_handle(fd);
        if (fh)
        {
            fh->sigio(nullptr);
        }
    }

    return totalReady;
}
