#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <pcap.h>

typedef struct mydev
{
    uint32_t ip_raw;
    char ip_str[INET_ADDRSTRLEN];
    char dev_name[IFNAMSIZ];
} mydev_t;

int ip_handler(const u_char *packet)
{
    struct iphdr *ip = NULL;

    if (NULL == packet)
    {
        printf("Somehow got a null packet pointer!!\n");
        return -1;
    }

    ip = (struct iphdr *)(packet + ETHER_HDR_LEN);

    switch (ip->protocol)
    {
    case IPPROTO_TCP:
        printf("TCP packet!\n");
        break;

    case IPPROTO_UDP:
        printf("UDP packet!\n");
        break;

    default:
        printf("Unhandled type!\n");
        break;
    }

    return 0;
}

/* This function can be used as a callback for pcap_loop() */
void link_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct ether_header *eth_header;
    /* The packet is larger than the ether_header struct,
       but we just want to look at the first part of the packet
       that contains the header. We force the compiler
       to treat the pointer to the packet as just a pointer
       to the ether_header. The data payload of the packet comes
       after the headers. Different packet types have different header
       lengths though, but the ethernet header is always the same (14 bytes) */
    eth_header = (struct ether_header *)packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP)
    {
        printf("IP \n");
        ip_handler(packet);
    }
    else if (ntohs(eth_header->ether_type) == ETHERTYPE_ARP)
    {
        printf("ARP\n");
    }
    else if (ntohs(eth_header->ether_type) == ETHERTYPE_REVARP)
    {
        printf("Reverse ARP\n");
    }
}

void get_dev_info(char *device)
{
    char ip[13] = {0};
    char subnet_mask[13] = {0};
    char error_buffer[PCAP_ERRBUF_SIZE]; /* Size defined in pcap.h */
    char *tmp = NULL;
    bpf_u_int32 ip_raw = 0;          /* IP address as integer */
    bpf_u_int32 subnet_mask_raw = 0; /* Subnet mask as integer */
    int lookup_return_code = 0;
    struct in_addr address = {0}; /* Used for both ip & subnet */

    /* Get device info */
    lookup_return_code = pcap_lookupnet(
        device,
        &ip_raw,
        &subnet_mask_raw,
        error_buffer);

    if (lookup_return_code == -1)
    {
        printf("%s\n", error_buffer);
        return;
    }

    /* Get ip in human readable form */
    address.s_addr = ip_raw;
    printf("x%08X\n", ip_raw);
    tmp = inet_ntoa(address);
    if (NULL == tmp)
    {
        perror("inet_ntoa"); /* print error */
        return;
    }
    strcpy(ip, tmp);

    /* Get subnet mask in human readable form */
    address.s_addr = subnet_mask_raw;
    tmp = inet_ntoa(address);
    if (NULL == tmp)
    {
        perror("inet_ntoa");
        return;
    }
    strcpy(subnet_mask, tmp);
    printf("---------------------\n");
    printf("Device: %s\n", device);
    printf("IP address: %s\n", ip);
    printf("Subnet mask: %s\n", subnet_mask);
}

void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header)
{
    printf("Packet capture length: %d\n", packet_header.caplen);
    printf("Packet total length %d\n", packet_header.len);
}

void open_then_single_pkt(char *device)
{
    const u_char *packet = NULL;
    char error_buffer[PCAP_ERRBUF_SIZE] = {0};
    int packet_count_limit = 1;
    int timeout_limit = 10000; /* In milliseconds */
    pcap_t *handle = NULL;
    struct pcap_pkthdr packet_header = {0};

    /* Open device for live capture */
    handle = pcap_open_live(
        device,
        BUFSIZ,
        packet_count_limit,
        timeout_limit,
        error_buffer);

    if (NULL == handle)
    {
        printf("%s\n", error_buffer);
        return;
    }

    /* Attempt to capture one packet. If there is no network traffic
     and the timeout is reached, it will return NULL */
    packet = pcap_next(handle, &packet_header);
    if (NULL == packet)
    {
        printf("No packet found.\n");
        return;
    }

    /* Our function to output some info */
    print_packet_info(packet, packet_header);
}

int main(int argc, char **argv)
{
    char error_buffer[PCAP_ERRBUF_SIZE]; /* Size defined in pcap.h */
    pcap_if_t *interfaces, *tmp;

    if (-1 == pcap_init(PCAP_CHAR_ENC_LOCAL, error_buffer))
    {
        printf("%s\n", error_buffer);
        return -1;
    }

    /* Find a device */
    if (pcap_findalldevs(&interfaces, error_buffer) == -1)
    {
        printf("Error in pcap findall devs");
        return -1;
    }

    printf("Interfaces:\n");
    for (tmp = interfaces; tmp; tmp = tmp->next)
    {
        printf("%s:", tmp->name);
        for (pcap_addr_t *a = tmp->addresses; a != NULL; a = a->next)
        {
            if (a->addr->sa_family == AF_INET)
            {
                printf(" %s\n", inet_ntoa(((struct sockaddr_in *)a->addr)->sin_addr));
            }
        }
        printf("\n");
    }
    int snapshot_len = 1028;
    int promiscuous = 0;
    int timeout = 1000;
    pcap_t *handle = NULL;

    handle = pcap_open_live(interfaces[0].name, snapshot_len, promiscuous, timeout, error_buffer);
    if (NULL == handle)
    {
        printf("Error during pcap_open_live...\n");
        printf("%s\n", error_buffer);
        return -1;
    }
    pcap_loop(handle, -1, link_handler, NULL);
    pcap_close(handle);
    handle = NULL;

    // open_then_single_pkt(interfaces[0].name);

    pcap_freealldevs(interfaces); // Clean up from find_alldevs
    return 0;
}