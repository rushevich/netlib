#include <cstddef>
#include <cstdint>

struct sockaddr {
    unsigned short sa_family; // macro: AF_INET, AF_INET6, for ipv4 and ipv6
    char sa_data[14];         // destination address and port number
};

// (IPv4 only--see struct sockaddr_in6 for IPv6)
struct in_addr {
    uint32_t s_addr; // also in network byte order, big endian
};
struct sockaddr_in { // this can be cast to a pointer to sockaddr!!
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number (must be in NBO (use htons))
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
// end of IPV4 shit

// (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4)
struct in6_addr {
    unsigned char   s6_addr[16];   // IPv6 address
};

struct sockaddr_in6 {
    uint16_t       sin6_family;   // address family, AF_INET6
    uint16_t       sin6_port;     // port, Network Byte Order
    uint32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    uint32_t       sin6_scope_id; // Scope ID
};
// end of IPv6 shit

struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};

// This storage struct can be used to store either ipv4 or ipv6 struct then cast based on a check to the address family
// struct sockaddr_storage {
//     sa_family_t  ss_family;     // address family

//     // all this is padding, implementation specific, ignore it:
//     char      __ss_pad1[_SS_PAD1SIZE];
//     int64_t   __ss_align;
//     char      __ss_pad2[_SS_PAD2SIZE];
//};

/*
  We can use another function: inet_pton() to convert an ip address
  to the data that we can pass around
  sockaddr_in sa;
  sockaddr_in6 sa6;
  inet_pton(AF_INET or AF_INET6, some ip ("10.10.10.10"), &(sa.sin_addr or
sa6.sin6_addr))
  think of pton as presentation to network
  returns -1 on error, or 0 on malformed input
  there is an equivalent inet_ntop
  // IPv4:

char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
struct sockaddr_in sa;      // pretend this is loaded with something

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

printf("The IPv4 address is: %s\n", ip4);


// IPv6:

char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
struct sockaddr_in6 sa6;    // pretend this is loaded with something

inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);

printf("The address is: %s\n", ip6);
 */
