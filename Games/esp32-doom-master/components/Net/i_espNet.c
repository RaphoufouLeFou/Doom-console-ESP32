#include "stdlib"
#include "string"
#include "stdio"
#include "lwip/opt.h"


#include "lwip/mem.h"


int SDLNet_ResolveHost(IPaddress *address, const char *host, Uint16 port);


int SDLNet_UDP_Bind(UDPsocket sock, int channel, const IPaddress *address){
    lwip_bind(sock, const struct sockaddr *name, socklen_t namelen);
}

void SDLNet_UDP_Unbind(UDPsocket sock, int channel);
sockaddr_in SDLNet_UDP_Open(Uint16 port);
void SDLNet_UDP_Close(UDPsocket sock);
int SDLNet_UDP_Recv(UDPsocket sock, UDPpacket *packet);
int SDLNet_UDP_Send(UDPsocket sock, int channel, UDPpacket *packet);
const char * SDLNet_ResolveIP(const IPaddress *ip);