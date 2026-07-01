#ifndef PS3_NET_H
#define PS3_NET_H

/*
 * Minimal PS3 networking stub header.
 *
 * The ps3dev image ships no usable networking library for this target, and
 * networking is not supported in this PS3 port. This header declares just
 * enough of a UDP socket/packet API that dragonfly-quake's net module
 * compiles; the corresponding .c file provides no-op stubs so the link
 * succeeds and the binary loads, with the network driver reporting failure
 * at runtime as expected by the rest of the engine.
 */
#include <stdint.h>
#include <stddef.h>

typedef uint32_t Uint32;
typedef uint16_t Uint16;
typedef uint8_t  Uint8;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Uint32 host;
    Uint16 port;
} IPaddress;

typedef struct _UDPsocket *UDPsocket;

typedef struct {
    int channel;
    Uint8 *data;
    int len;
    int maxlen;
    int status;
    IPaddress address;
} UDPpacket;

#define INADDR_ANY      0x00000000
#define INADDR_BROADCAST 0xFFFFFFFF
#define INADDR_LOOPBACK 0x7F000001   /* 127.0.0.1 */
#define INADDR_NONE     0xFFFFFFFF

/* net_udp.c calls gethostname() directly. PS3 newlib doesn't always declare
 * it in the headers; declare it here so the call is not implicit. */
extern int gethostname(char *name, size_t len);

#define PS3NET_MAX_UDPCHANNELS 1
#define PS3NET_MAX_UDPADDRESSES 1

int  PS3Net_Init(void);
void PS3Net_Quit(void);
const char *PS3Net_GetError(void);

int PS3Net_ResolveHost(IPaddress *address, const char *host, Uint16 port);
const char *PS3Net_ResolveIP(const IPaddress *ip);

UDPsocket PS3Net_UDP_Open(Uint16 port);
void PS3Net_UDP_Close(UDPsocket sock);

int PS3Net_UDP_Bind(UDPsocket sock, int channel, const IPaddress *address);
void PS3Net_UDP_Unbind(UDPsocket sock, int channel);
IPaddress *PS3Net_UDP_GetPeerAddress(UDPsocket sock, int channel);

int PS3Net_UDP_Send(UDPsocket sock, int channel, UDPpacket *packet);
int PS3Net_UDP_Recv(UDPsocket sock, UDPpacket *packet);

UDPpacket *PS3Net_AllocPacket(int size);
void PS3Net_FreePacket(UDPpacket *packet);
UDPpacket **PS3Net_AllocPacketV(int howmany, int size);
void PS3Net_FreePacketV(UDPpacket **packets);

int PS3Net_GetLocalAddresses(IPaddress *addresses, int maxcount);

/* Inline byte-order helpers (big-endian / network order). */
static inline Uint32 PS3Net_Read32(const void *area) {
    const Uint8 *p = (const Uint8 *)area;
    return ((Uint32)p[0] << 24) | ((Uint32)p[1] << 16)
         | ((Uint32)p[2] << 8)  |  (Uint32)p[3];
}
static inline void PS3Net_Write32(Uint32 value, void *area) {
    Uint8 *p = (Uint8 *)area;
    p[0] = (Uint8)(value >> 24);
    p[1] = (Uint8)(value >> 16);
    p[2] = (Uint8)(value >> 8);
    p[3] = (Uint8)(value);
}
static inline Uint16 PS3Net_Read16(const void *area) {
    const Uint8 *p = (const Uint8 *)area;
    return (Uint16)(((Uint16)p[0] << 8) | (Uint16)p[1]);
}
static inline void PS3Net_Write16(Uint16 value, void *area) {
    Uint8 *p = (Uint8 *)area;
    p[0] = (Uint8)(value >> 8);
    p[1] = (Uint8)(value);
}

#ifdef __cplusplus
}
#endif

#endif /* PS3_NET_H */
