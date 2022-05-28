#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    u_int32_t ipv4_h = 0, ipv4_n = 0;
    u_int16_t port_h = 0, port_n = 0;

    if (argc < 3)
        return 0;
    char *ipv4_str = argv[1];
    char *port_str = argv[2];

    //ip_str to int
    unsigned char *ipv4_n_cbc = (unsigned char *) &ipv4_n;

    char *pEnd = ipv4_str;
    for (int i = 0; i < 4; ++i) {//read ipv4_n
        u_int8_t ip_byte = (u_int8_t) strtol(pEnd, &pEnd, 10);
        ipv4_n_cbc[i] |= ip_byte;
        pEnd++;
    }
    ipv4_h = ntohl(ipv4_n);
    unsigned char *ipv4_h_cbc = (unsigned char *) &ipv4_h;

    //port_str to int
    u_int16_t port = (u_int16_t) strtol(port_str, (char **) NULL, 10);
    port_n = htons(port);
    port_h = ntohs(port_n);

    unsigned char *port_h_cbc = (unsigned char *) &port_h;
    unsigned char *port_n_cbc = (unsigned char *) &port_n;

    printf("[ipv4_n_cbc_d]%d.%d.%d.%d\n", ipv4_n_cbc[0], ipv4_n_cbc[1], ipv4_n_cbc[2], ipv4_n_cbc[3]);
    printf("[ipv4_n_cbc_x]%02X%02X%02X%02X\n", ipv4_n_cbc[0], ipv4_n_cbc[1], ipv4_n_cbc[2], ipv4_n_cbc[3]);
    printf("[ipv4_n_asw_x]%08X\n", ipv4_n);
    printf("[port_n_cbc_x]%02X%02X\n", port_n_cbc[0], port_n_cbc[1]);
    printf("[port_n_asw_x]%04X\n", port_n);
    printf("[port_n_asw_d]%d\n", port_n);
    printf("[ipv4_h_cbc_d]%d.%d.%d.%d\n", ipv4_h_cbc[0], ipv4_h_cbc[1], ipv4_h_cbc[2], ipv4_h_cbc[3]);
    printf("[ipv4_h_cbc_x]%02X%02X%02X%02X\n", ipv4_h_cbc[0], ipv4_h_cbc[1], ipv4_h_cbc[2], ipv4_h_cbc[3]);
    printf("[ipv4_h_asw_x]%08X\n", ipv4_h);
    printf("[port_h_cbc_x]%02X%02X\n", port_h_cbc[0], port_h_cbc[1]);
    printf("[port_h_asw_x]%04X\n", port_h);
    printf("[port_h_asw_d]%d\n", port_h);

    return 0;
}