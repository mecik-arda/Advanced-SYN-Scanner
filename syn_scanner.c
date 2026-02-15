#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
    struct tcphdr tcp;
};

unsigned short checksum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    unsigned short oddbyte;
    short answer;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;
    return (answer);
}

void print_usage(char *prog_name) {
    printf("Usage: sudo %s -s <Source IP> -t <Target IP> -p <Start Port> -e <End Port>\n", prog_name);
}

int main(int argc, char *argv[]) {
    char *source_ip = NULL;
    char *target_ip = NULL;
    int start_port = 0, end_port = 0, opt;

    while ((opt = getopt(argc, argv, "s:t:p:e:h")) != -1) {
        switch (opt) {
            case 's': source_ip = optarg; break;
            case 't': target_ip = optarg; break;
            case 'p': start_port = atoi(optarg); break;
            case 'e': end_port = atoi(optarg); break;
            case 'h': print_usage(argv[0]); exit(0);
            default: exit(1);
        }
    }

    if (!source_ip || !target_ip || start_port <= 0 || end_port < start_port) {
        print_usage(argv[0]);
        exit(1);
    }

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s == -1) {
        perror("Socket error");
        exit(1);
    }

    int one = 1;
    setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000; 
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    printf("Starting SYN scan on %s from port %d to %d...\n", target_ip, start_port, end_port);

    for (int port = start_port; port <= end_port; port++) {
        char datagram[4096];
        memset(datagram, 0, 4096);
        struct iphdr *iph = (struct iphdr *)datagram;
        struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof(struct ip));
        struct sockaddr_in sin;
        struct pseudo_header psh;

        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = inet_addr(target_ip);

        iph->ihl = 5;
        iph->version = 4;
        iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
        iph->id = htons(54321 + port);
        iph->ttl = 255;
        iph->protocol = IPPROTO_TCP;
        iph->saddr = inet_addr(source_ip);
        iph->daddr = sin.sin_addr.s_addr;
        iph->check = checksum((unsigned short *)datagram, iph->tot_len >> 1);

        tcph->source = htons(12345 + (port % 1000));
        tcph->dest = htons(port);
        tcph->seq = 0;
        tcph->ack_seq = 0;
        tcph->doff = 5;
        tcph->syn = 1;
        tcph->window = htons(5840);
        tcph->check = 0;

        psh.source_address = inet_addr(source_ip);
        psh.dest_address = sin.sin_addr.s_addr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(sizeof(struct tcphdr));
        memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));
        tcph->check = checksum((unsigned short *)&psh, sizeof(struct pseudo_header));

        sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));

        char buffer[65536];
        struct sockaddr_in saddr;
        socklen_t saddr_len = sizeof(saddr);

        while (1) {
            int data_size = recvfrom(s, buffer, 65536, 0, (struct sockaddr *)&saddr, &saddr_len);
            if (data_size < 0) break; 

            struct iphdr *recv_iph = (struct iphdr *)buffer;
            if (recv_iph->saddr == sin.sin_addr.s_addr) {
                struct tcphdr *recv_tcph = (struct tcphdr *)(buffer + (recv_iph->ihl * 4));
                if (ntohs(recv_tcph->source) == port) {
                    if (recv_tcph->syn == 1 && recv_tcph->ack == 1) {
                        printf("[+] Port %d is OPEN\n", port);
                    } else if (recv_tcph->rst == 1) {
                        // Port closed, we can stay silent or print it
                    }
                    break;
                }
            }
        }
    }

    printf("Scan completed.\n");
    close(s);
    return 0;
}
