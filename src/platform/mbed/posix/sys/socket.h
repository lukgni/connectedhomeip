#ifndef MBED_POSIX_SYS_SOCKET_H
#define MBED_POSIX_SYS_SOCKET_H

#include <netsocket/Socket.h>
#include <utils/byteorder.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Protocol families. */
#define PF_UNSPEC 0      /**< Unspecified protocol family.  */
#define PF_INET 1        /**< IP protocol family version 4. */
#define PF_INET6 2       /**< IP protocol family version 6. */
#define PF_PACKET 3      /**< Packet family.                */
#define PF_CAN 4         /**< Controller Area Network.      */
#define PF_NET_MGMT 5    /**< Network management info.      */
#define PF_LOCAL 6       /**< Inter-process communication   */
#define PF_UNIX PF_LOCAL /**< Inter-process communication   */

/* Address families. */
#define AF_UNSPEC PF_UNSPEC     /**< Unspecified address family.   */
#define AF_INET PF_INET         /**< IP protocol family version 4. */
#define AF_INET6 PF_INET6       /**< IP protocol family version 6. */
#define AF_PACKET PF_PACKET     /**< Packet family.                */
#define AF_CAN PF_CAN           /**< Controller Area Network.      */
#define AF_NET_MGMT PF_NET_MGMT /**< Network management info.      */
#define AF_LOCAL PF_LOCAL       /**< Inter-process communication   */
#define AF_UNIX PF_UNIX         /**< Inter-process communication   */

/** Protocol numbers from IANA/BSD */
enum net_ip_protocol
{
    IPPROTO_IP     = 0,   /**< IP protocol (pseudo-val for setsockopt() */
    IPPROTO_ICMP   = 1,   /**< ICMP protocol   */
    IPPROTO_TCP    = 6,   /**< TCP protocol    */
    IPPROTO_UDP    = 17,  /**< UDP protocol    */
    IPPROTO_IPV6   = 41,  /**< IPv6 protocol   */
    IPPROTO_ICMPV6 = 58,  /**< ICMPv6 protocol */
    IPPROTO_RAW    = 255, /**< RAW IP packets  */
};

/** Protocol numbers for TLS protocols */
enum net_ip_protocol_secure
{
    IPPROTO_TLS_1_0  = 256, /**< TLS 1.0 protocol */
    IPPROTO_TLS_1_1  = 257, /**< TLS 1.1 protocol */
    IPPROTO_TLS_1_2  = 258, /**< TLS 1.2 protocol */
    IPPROTO_DTLS_1_0 = 272, /**< DTLS 1.0 protocol */
    IPPROTO_DTLS_1_2 = 273, /**< DTLS 1.2 protocol */
};

/** Socket type */
enum net_sock_type
{
    SOCK_STREAM = 1, /**< Stream socket type   */
    SOCK_DGRAM,      /**< Datagram socket type */
    SOCK_RAW         /**< RAW socket type      */
};

/** @brief Convert 16-bit value from network to host byte order.
 *
 * @param x The network byte order value to convert.
 *
 * @return Host byte order value.
 */
#define ntohs(x) sys_be16_to_cpu(x)

/** @brief Convert 32-bit value from network to host byte order.
 *
 * @param x The network byte order value to convert.
 *
 * @return Host byte order value.
 */
#define ntohl(x) sys_be32_to_cpu(x)

/** @brief Convert 64-bit value from network to host byte order.
 *
 * @param x The network byte order value to convert.
 *
 * @return Host byte order value.
 */
#define ntohll(x) sys_be64_to_cpu(x)

/** @brief Convert 16-bit value from host to network byte order.
 *
 * @param x The host byte order value to convert.
 *
 * @return Network byte order value.
 */
#define htons(x) sys_cpu_to_be16(x)

/** @brief Convert 32-bit value from host to network byte order.
 *
 * @param x The host byte order value to convert.
 *
 * @return Network byte order value.
 */
#define htonl(x) sys_cpu_to_be32(x)

/** @brief Convert 64-bit value from host to network byte order.
 *
 * @param x The host byte order value to convert.
 *
 * @return Network byte order value.
 */
#define htonll(x) sys_cpu_to_be64(x)

/** IPv6 address struct */
struct in6_addr
{
    union
    {
        uint8_t s6_addr[16];
        uint16_t s6_addr16[8]; /* In big endian */
        uint32_t s6_addr32[4]; /* In big endian */
    };
};

/** IPv4 address struct */
struct in_addr
{
    union
    {
        uint8_t s4_addr[4];
        uint16_t s4_addr16[2]; /* In big endian */
        uint32_t s4_addr32[1]; /* In big endian */
        uint32_t s_addr;       /* In big endian, for POSIX compatibility. */
    };
};

/** Socket address family type */
typedef unsigned short int sa_family_t;

/** Length of a socket address */
typedef size_t socklen_t;

/*
 * Note that the sin_port and sin6_port are in network byte order
 * in various sockaddr* structs.
 */

/** Socket address struct for IPv6. */
struct sockaddr_in6
{
    sa_family_t sin6_family;   /* AF_INET6               */
    uint16_t sin6_port;        /* Port number            */
    uint32_t sin6_flowinfo;    /* IPv6 flow information  */
    struct in6_addr sin6_addr; /* IPv6 address           */
    uint8_t sin6_scope_id;     /* interfaces for a scope */
};

struct sockaddr_in6_ptr
{
    sa_family_t sin6_family;     /* AF_INET6               */
    uint16_t sin6_port;          /* Port number            */
    struct in6_addr * sin6_addr; /* IPv6 address           */
    uint8_t sin6_scope_id;       /* interfaces for a scope */
};

/** Socket address struct for IPv4. */
struct sockaddr_in
{
    sa_family_t sin_family;  /* AF_INET      */
    uint16_t sin_port;       /* Port number  */
    struct in_addr sin_addr; /* IPv4 address */
};

struct sockaddr_in_ptr
{
    sa_family_t sin_family;    /* AF_INET      */
    uint16_t sin_port;         /* Port number  */
    struct in_addr * sin_addr; /* IPv4 address */
};

/** Socket address struct for packet socket. */
struct sockaddr_ll
{
    sa_family_t sll_family; /* Always AF_PACKET        */
    uint16_t sll_protocol;  /* Physical-layer protocol */
    int sll_ifindex;        /* Interface number        */
    uint16_t sll_hatype;    /* ARP hardware type       */
    uint8_t sll_pkttype;    /* Packet type             */
    uint8_t sll_halen;      /* Length of address       */
    uint8_t sll_addr[8];    /* Physical-layer address  */
};

struct sockaddr_ll_ptr
{
    sa_family_t sll_family; /* Always AF_PACKET        */
    uint16_t sll_protocol;  /* Physical-layer protocol */
    int sll_ifindex;        /* Interface number        */
    uint16_t sll_hatype;    /* ARP hardware type       */
    uint8_t sll_pkttype;    /* Packet type             */
    uint8_t sll_halen;      /* Length of address       */
    uint8_t * sll_addr;     /* Physical-layer address  */
};

struct msghdr
{
    void * msg_name;        /* optional socket address */
    socklen_t msg_namelen;  /* size of socket address */
    struct iovec * msg_iov; /* scatter/gather array */
    size_t msg_iovlen;      /* number of elements in msg_iov */
    void * msg_control;     /* ancillary data */
    size_t msg_controllen;  /* ancillary data buffer len */
    int msg_flags;          /* flags on received message */
};

struct cmsghdr
{
    socklen_t cmsg_len; /* Number of bytes, including header */
    int cmsg_level;     /* Originating protocol */
    int cmsg_type;      /* Protocol-specific type */
};

/* Alignment for headers and data. These are arch specific but define
 * them here atm if not found alredy.
 */
#define ALIGN_H(size) (((size) + sizeof(long) - 1U) & ~(sizeof(long) - 1U))
#define ALIGN_D(size) ALIGN_H(size)

#define CMSG_FIRSTHDR(mhdr)                                                                                                        \
    ((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? (struct cmsghdr *) (mhdr)->msg_control : (struct cmsghdr *) NULL)

#define CMSG_NXTHDR(mhdr, cmsg)                                                                                                    \
    (((cmsg) == NULL) ? CMSG_FIRSTHDR(mhdr)                                                                                        \
                      : (((uint8_t *) (cmsg) + ALIGN_H((cmsg)->cmsg_len) + ALIGN_D(sizeof(struct cmsghdr)) >                       \
                          (uint8_t *) ((mhdr)->msg_control) + (mhdr)->msg_controllen)                                              \
                             ? (struct cmsghdr *) NULL                                                                             \
                             : (struct cmsghdr *) ((void *) ((uint8_t *) (cmsg) + ALIGN_H((cmsg)->cmsg_len)))))

#define CMSG_DATA(cmsg) ((void *) ((uint8_t *) (cmsg) + ALIGN_D(sizeof(struct cmsghdr))))

#define CMSG_SPACE(length) (ALIGN_D(sizeof(struct cmsghdr)) + ALIGN_H(length))

#define CMSG_LEN(length) (ALIGN_D(sizeof(struct cmsghdr)) + length)

/* Packet types.  */
#define PACKET_HOST 0      /* To us            */
#define PACKET_BROADCAST 1 /* To all           */
#define PACKET_MULTICAST 2 /* To group         */
#define PACKET_OTHERHOST 3 /* To someone else  */
#define PACKET_OUTGOING 4  /* Originated by us */
#define PACKET_LOOPBACK 5
#define PACKET_FASTROUTE 6

/* Note: These macros are defined in a specific order.
 * The largest sockaddr size is the last one.
 */
#if defined(CONFIG_NET_IPV4)
#undef NET_SOCKADDR_MAX_SIZE
#undef NET_SOCKADDR_PTR_MAX_SIZE
#define NET_SOCKADDR_MAX_SIZE (sizeof(struct sockaddr_in))
#define NET_SOCKADDR_PTR_MAX_SIZE (sizeof(struct sockaddr_in_ptr))
#endif

#if defined(CONFIG_NET_SOCKETS_PACKET)
#undef NET_SOCKADDR_MAX_SIZE
#undef NET_SOCKADDR_PTR_MAX_SIZE
#define NET_SOCKADDR_MAX_SIZE (sizeof(struct sockaddr_ll))
#define NET_SOCKADDR_PTR_MAX_SIZE (sizeof(struct sockaddr_ll_ptr))
#endif

#if defined(CONFIG_NET_IPV6)
#undef NET_SOCKADDR_MAX_SIZE
#define NET_SOCKADDR_MAX_SIZE (sizeof(struct sockaddr_in6))
#if !defined(CONFIG_NET_SOCKETS_PACKET)
#undef NET_SOCKADDR_PTR_MAX_SIZE
#define NET_SOCKADDR_PTR_MAX_SIZE (sizeof(struct sockaddr_in6_ptr))
#endif
#endif

#if !defined(CONFIG_NET_IPV4)
#if !defined(CONFIG_NET_IPV6)
#if !defined(CONFIG_NET_SOCKETS_PACKET)
#define NET_SOCKADDR_MAX_SIZE (sizeof(struct sockaddr_in6))
#define NET_SOCKADDR_PTR_MAX_SIZE (sizeof(struct sockaddr_in6_ptr))
#endif
#endif
#endif

/** Generic sockaddr struct. Must be cast to proper type. */
struct sockaddr
{
    sa_family_t sa_family;
    char data[NET_SOCKADDR_MAX_SIZE - sizeof(sa_family_t)];
};

struct sockaddr_ptr
{
    sa_family_t family;
    char data[NET_SOCKADDR_PTR_MAX_SIZE - sizeof(sa_family_t)];
};

/* Same as sockaddr in our case */
struct sockaddr_storage
{
    sa_family_t ss_family;
    char data[NET_SOCKADDR_MAX_SIZE - sizeof(sa_family_t)];
};

/* Socket address struct for UNIX domain sockets */
struct sockaddr_un
{
    sa_family_t sun_family; /* AF_UNIX */
    char sun_path[NET_SOCKADDR_MAX_SIZE - sizeof(sa_family_t)];
};

struct net_addr
{
    sa_family_t family;
    union
    {
        struct in6_addr in6_addr;
        struct in_addr in_addr;
    };
};

#define IN6ADDR_ANY_INIT                                                                                                           \
    {                                                                                                                              \
        {                                                                                                                          \
            {                                                                                                                      \
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                                                                     \
            }                                                                                                                      \
        }                                                                                                                          \
    }
#define IN6ADDR_LOOPBACK_INIT                                                                                                      \
    {                                                                                                                              \
        {                                                                                                                          \
            {                                                                                                                      \
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1                                                                     \
            }                                                                                                                      \
        }                                                                                                                          \
    }

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

/** Max length of the IPv4 address as a string. Defined by POSIX. */
#define INET_ADDRSTRLEN 16
/** Max length of the IPv6 address as a string. Takes into account possible
 * mapped IPv4 addresses.
 */
#define INET6_ADDRSTRLEN 46

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h (checked in init.c)
 */
#define SO_REUSEADDR 0x0004 /* Allow local address reuse */
#define SO_KEEPALIVE 0x0008 /* keep connections alive */
#define SO_BROADCAST 0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */

/*
 * Additional options, not kept in so_options.
 */
#define SO_DEBUG 0x0001       /* Unimplemented: turn on debugging info recording */
#define SO_ACCEPTCONN 0x0002  /* socket has had listen() */
#define SO_DONTROUTE 0x0010   /* Unimplemented: just use interface addresses */
#define SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define SO_LINGER 0x0080      /* linger on close if data present */
#define SO_DONTLINGER ((int) (~SO_LINGER))
#define SO_OOBINLINE 0x0100    /* Unimplemented: leave received OOB data in line */
#define SO_REUSEPORT 0x0200    /* Unimplemented: allow local address & port reuse */
#define SO_SNDBUF 0x1001       /* Unimplemented: send buffer size */
#define SO_RCVBUF 0x1002       /* receive buffer size */
#define SO_SNDLOWAT 0x1003     /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT 0x1004     /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO 0x1005     /* send timeout */
#define SO_RCVTIMEO 0x1006     /* receive timeout */
#define SO_ERROR 0x1007        /* get error status and clear */
#define SO_TYPE 0x1008         /* get socket type */
#define SO_CONTIMEO 0x1009     /* Unimplemented: connect timeout */
#define SO_NO_CHECK 0x100a     /* don't create UDP checksum */
#define SO_BINDTODEVICE 0x100b /* bind to device */

/*
 * Structure used for manipulating linger option.
 */
struct linger
{
    int l_onoff;  /* option on/off */
    int l_linger; /* linger time in seconds */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET 0xfff /* options for socket level */

/* File status flags and file access modes for fnctl,
   these are bits in an int. */
#ifndef O_NONBLOCK
#define O_NONBLOCK 1 /* nonblocking I/O */
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK /* same as O_NONBLOCK, for compatibility */
#endif
#ifndef O_RDONLY
#define O_RDONLY 2
#endif
#ifndef O_WRONLY
#define O_WRONLY 4
#endif
#ifndef O_RDWR
#define O_RDWR (O_RDONLY | O_WRONLY)
#endif

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

/*
 * Options for level IPPROTO_TCP
 */
#define TCP_NODELAY 0x01   /* don't delay send to coalesce packets */
#define TCP_KEEPALIVE 0x02 /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */
#define TCP_KEEPIDLE 0x03  /* set pcb->keep_idle  - Same as TCP_KEEPALIVE, but use seconds for get/setsockopt */
#define TCP_KEEPINTVL 0x04 /* set pcb->keep_intvl - Use seconds for get/setsockopt */
#define TCP_KEEPCNT 0x05   /* set pcb->keep_cnt   - Use number of probes sent for get/setsockopt */

/* Set socket options argument */
#define IFNAMSIZ 6
struct ifreq
{
    char ifr_name[IFNAMSIZ]; /* Interface name */
};

/* Flags we can use with send and recv. */
#define MSG_PEEK 0x01
#define MSG_WAITALL 0x02
#define MSG_OOB 0x04
#define MSG_DONTWAIT 0x08
#define MSG_MORE 0x10
#define MSG_NOSIGNAL 0x20

#if !defined(iovec)
struct iovec
{
    void * iov_base;
    size_t iov_len;
};
#endif

/*
 * Options and types related to multicast membership
 */
#define IP_ADD_MEMBERSHIP 3
#define IP_DROP_MEMBERSHIP 4

typedef struct ip_mreq
{
    struct in_addr imr_multiaddr; /* IP multicast address of group */
    struct in_addr imr_interface; /* local IP address of interface */
} ip_mreq;

static inline int socket(int family, int type, int proto)
{
    return 0;
}

static inline int socketpair(int family, int type, int proto, int sv[2])
{
    return 0;
}

static inline int shutdown(int sock, int how)
{
    return 0;
}

static inline int bind(int sock, const struct sockaddr * addr, socklen_t addrlen)
{
    return 0;
}

static inline int connect(int sock, const struct sockaddr * addr, socklen_t addrlen)
{
    return 0;
}

static inline int listen(int sock, int backlog)
{
    return 0;
}

static inline int accept(int sock, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

static inline ssize_t send(int sock, const void * buf, size_t len, int flags)
{
    return 0;
}

static inline ssize_t recv(int sock, void * buf, size_t max_len, int flags)
{
    return 0;
}

static inline ssize_t sendto(int sock, const void * buf, size_t len, int flags, const struct sockaddr * dest_addr,
                             socklen_t addrlen)
{
    return 0;
}

static inline ssize_t sendmsg(int sock, const struct msghdr * message, int flags)
{
    return 0;
}

static inline ssize_t recvfrom(int sock, void * buf, size_t max_len, int flags, struct sockaddr * src_addr, socklen_t * addrlen)
{
    return 0;
}

static inline int getsockopt(int sock, int level, int optname, void * optval, socklen_t * optlen)
{
    return 0;
}

static inline int setsockopt(int sock, int level, int optname, const void * optval, socklen_t optlen)
{
    return 0;
}

static inline int getsockname(int sock, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

static inline int getpeername(int sockfd, struct sockaddr * addr, socklen_t * addrlen)
{
    return 0;
}

static inline ssize_t recvmsg(int socket, struct msghdr * message, int flags)
{
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MBED_POSIX_SYS_SOCKET_H */
