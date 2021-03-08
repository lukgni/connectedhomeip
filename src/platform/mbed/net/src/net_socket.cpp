#include <net_socket.h>

#define MAX_SOCKET 5

#define TCP_SOCKET SOCK_STREAM
#define UDP_SOCKET SOCK_DGRAM
#define SOCKET_NOT_INITIALIZED 0
#define ERR_NO_MEMORY -1
#define ERR_OPEN -2
#define ERR_NO_SOCKET -3

BSDSocket sockets[MAX_SOCKET];

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

void Sockaddr2Netsocket(SocketAddress * dst, struct sockaddr * src)
{
    sockaddr_in * addr = reinterpret_cast<sockaddr_in *>(src);
    dst->set_ip_bytes((const void *) addr->sin_addr.s_addr, Inet2Nsapi(src->sa_family));
}

Socket * getSocket(int id)
{
    int type = 1;
    switch (type)
    {
    case TCP_SOCKET:
        return &sockets[id].tcpSocket;
    case UDP_SOCKET:
        return &sockets[id].udpSocket;
    default:
        return nullptr;
    }
}
int findMemForSocket()
{
    int id = ERR_NO_MEMORY;
    for (int i = 0; i < MAX_SOCKET; i++)
    {
        if (sockets[i].type == SOCKET_NOT_INITIALIZED)
        {
            id = i;
            break;
        }
    }
    return id;
}

int close(int id)
{
    auto * sock = getSocket(id);
    sock->~Socket();
    sockets[id].type = SOCKET_NOT_INITIALIZED;
}

int mbed_socket(int family, int type, int proto)
{
    int id = findMemForSocket();
    if (id == ERR_NO_MEMORY)
    {
        return ERR_NO_MEMORY;
    }

    switch (type)
    {
    case TCP_SOCKET: {
        TCPSocket * tcpSocket = new (&sockets[id].tcpSocket) TCPSocket();
        if (tcpSocket->open(NetworkInterface::get_default_instance()) != NSAPI_ERROR_OK)
        {
            tcpSocket->~TCPSocket();
            sockets[id].type = SOCKET_NOT_INITIALIZED;
            return ERR_OPEN;
        }
        sockets[id].type = TCP_SOCKET;
    }
    break;
    case UDP_SOCKET: {
        UDPSocket * udpSocket = new (&sockets[id].udpSocket) UDPSocket();
        if (udpSocket->open(NetworkInterface::get_default_instance()) != NSAPI_ERROR_OK)
        {
            udpSocket->~UDPSocket();
            sockets[id].type = SOCKET_NOT_INITIALIZED;
            return ERR_OPEN;
        }
        sockets[id].type = UDP_SOCKET;
    }
    break;
    default:
        break;
    };

    return id;
}

int mbed_socketpair(int family, int type, int proto, int sv[2])
{
    return 0;
}

int mbed_shutdown(int sock, int how)
{
    return 0;
}

int mbed_bind(int sock, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) addr);

    return socket->bind(sockAddr);
}

int mbed_connect(int sock, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) addr);

    return socket->connect(sockAddr);
}

int mbed_listen(int sock, int backlog)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }

    return socket->listen(backlog);
}

int mbed_accept(int sock, struct sockaddr * addr, socklen_t * addrlen)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }

    nsapi_error_t error;
    int id                = findMemForSocket();
    TCPSocket * tcpSocket = new (&sockets[id].tcpSocket) TCPSocket();
    tcpSocket             = static_cast<TCPSocket *>(socket->accept(&error));

    if (&sockets[id].tcpSocket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    if (sockets[id].tcpSocket.open(NetworkInterface::get_default_instance()) != NSAPI_ERROR_OK)
    {
        sockets[id].tcpSocket.~TCPSocket();
        sockets[id].type = SOCKET_NOT_INITIALIZED;
        return ERR_OPEN;
    }
    sockets[id].type = TCP_SOCKET;
    return id;
}

ssize_t mbed_send(int sock, const void * buf, size_t len, int flags)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    return socket->send(buf, len);
}

ssize_t mbed_recv(int sock, void * buf, size_t max_len, int flags)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    return socket->recv(buf, max_len);
}

ssize_t mbed_sendto(int sock, const void * buf, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, (struct sockaddr *) dest_addr);

    return socket->sendto(sockAddr, buf, len);
}

ssize_t mbed_sendmsg(int sock, const struct msghdr * message, int flags)
{
    return 0;
}

ssize_t mbed_recvfrom(int sock, void * buf, size_t max_len, int flags, struct sockaddr * src_addr, socklen_t * addrlen)
{
    auto * socket = getSocket(sock);
    if (socket == nullptr)
    {
        return ERR_NO_SOCKET;
    }
    SocketAddress sockAddr;
    Sockaddr2Netsocket(&sockAddr, src_addr);

    return socket->recvfrom(&sockAddr, buf, max_len);
}

int mbed_getsockopt(int sock, int level, int optname, void * optval, socklen_t * optlen)
{
    return 0;
}

int mbed_setsockopt(int sock, int level, int optname, const void * optval, socklen_t optlen)
{
    return 0;
}

int mbed_getsockname(int sock, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

int mbed_getpeername(int sockfd, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

ssize_t mbed_recvmsg(int socket, struct msghdr * message, int flags)
{
    return 0;
}
