#define ENET_IMPLEMENTATION
#include <stdio.h>
#include "enet.h"


int main(int argc, char ** argv){

    if (enet_initialize () != 0) {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 7777; /* Bind the server to port 7777. */

    #define MAX_CLIENTS 32

    /* create a server */
    ENetHost * server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);

    if (server == NULL) {
        printf("An error occurred while trying to create an ENet server host.\n");
        return 1;
    }

    printf("Started a server...\n");

    ENetEvent event;
    char peer_ip[20];
    int ret = 0;

    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    while ((ret = enet_host_service(server, &event, 10000)) > 0) {

        memset(peer_ip, 0, sizeof(peer_ip));

        switch (event.type) {

            case ENET_EVENT_TYPE_CONNECT:

                enet_address_get_host_ip_new(&event.peer->address, peer_ip, sizeof(peer_ip));
                printf("A new client connected from %s:%u.\n",  peer_ip, event.peer->address.port);

                /* Store any relevant client information here. */
                event.peer->data = "Client information";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                        event.packet->dataLength,
                        event.packet->data,
                        (char*)event.peer->data,
                        event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", (char*)event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                printf("%s disconnected due to timeout.\n", (char*)event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
    printf("Ret: %d", ret);
    enet_host_destroy(server);
    enet_deinitialize();
    return 0;

}

