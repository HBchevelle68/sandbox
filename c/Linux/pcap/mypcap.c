#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <pcap.h>

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

void single_pcap(char *device)
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
    pcap_if_t *interfaces, *temp;

    /* Find a device */
    if (pcap_findalldevs(&interfaces, error_buffer) == -1)
    {
        printf("Error in pcap findall devs");
        return -1;
    }

    printf("Interfaces:\n");
    for (temp = interfaces; temp; temp = temp->next)
    {
        get_dev_info(temp->name);
    }
    single_pcap(interfaces[0].name);

    pcap_freealldevs(interfaces); // Clean up from find_alldevs
    return 0;
}