#include <sys/param.h>
#include <sys/queue.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#define	_WANT_SOCKET
#include <sys/socketvar.h>
#include <sys/sysctl.h>

#include <net/route.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_carp.h>
#ifdef INET6
#include <netinet/ip6.h>
#endif /* INET6 */
#include <netinet/in_pcb.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>
#include <netinet/igmp_var.h>
#include <netinet/ip_var.h>
#include <netinet/pim_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_seq.h>
#define	TCPSTATES
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <libutil.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>



int main(){
    char* mibvar = "net.inet.tcp.pcblist";
    char* buf =  NULL;
    size_t len = 0 ;


    // Get size of what the kernel will return back
    if (sysctlbyname(mibvar, 0, &len, 0, 0) < 0) {
        if (errno != ENOENT){
	    printf("Error calling sysctlbyname()\n");
            return -1;
	}
    }

    if ((buf = malloc(len)) == NULL) {
        printf("Error allocating buffer size %zu\n", len);
    }

    if (sysctlbyname(mibvar, buf, &len, 0, 0) < 0) {
        printf("Error calling sysctlbyname().2\n");
        free(buf);
        return -1;
    }

    // Placing these closer to where they are being used
    struct xtcpcb *tp;
    struct xinpcb *inp;
    struct xinpgen *xig, *oxig;
    struct xsocket *so;

    // Taken from FreeBSD implementation...
    oxig = xig = (struct xinpgen *)buf;

    // Iterate through
    for (xig = (struct xinpgen*)((char *)xig + xig->xig_len);
	        xig->xig_len > sizeof(struct xinpgen);
		xig = (struct xinpgen *)((char *)xig + xig->xig_len)) {
    
        // Just using TCP for now
	// This is how netstat in freebsd works at least...
        tp = (struct xtcpcb *)xig;
	inp = &tp->xt_inp;	
	


    }
    free(buf); 
    return 0;
}
