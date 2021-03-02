#ifndef MBED_NET_NET_IF_H
#define MBED_NET_NET_IF_H

#include <byteorder.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MBED_MAX_INTERFACES_NUM 1
#define MBED_NET_IF_LIST_SIZE (MBED_MAX_INTERFACES_NUM + 1)

struct if_nameindex * mbed_if_nameindex(void);
char * mbed_if_indextoname(unsigned int ifindex, char * ifname);
unsigned int mbed_if_nametoindex(const char * ifname);
void mbed_if_freenameindex(struct if_nameindex * ptr);

int mbed_getifaddrs(struct ifaddrs ** ifap);
void mbed_freeifaddrs(struct ifaddrs * ifp);

char * mbed_inet_ntop(sa_family_t family, const void * src, char * dst, size_t size);
int mbed_inet_pton(sa_family_t family, const char * src, void * dst);

#ifdef __cplusplus
}
#endif

#endif /* MBED_NET_NET_IF_H */