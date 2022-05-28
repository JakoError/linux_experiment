#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>

#define PORT 12345
#define MAX_CMD_STR 100

int main(int argc, char **argv) {
    int client_fd;
    long num_bytes;
    char buf[MAX_CMD_STR];
    struct sockaddr_in server_addr;
    if (argc != 3) {
        printf("usages %s <server IP address> <server port>\n", argv[0]);
        exit(0);
    }

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Create socket failed.");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;


    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0) {
        perror("Server IP Address Error:\n");
        exit(1);
    }

    long tmp = strtol(argv[2], NULL, 10);
    if (tmp > USHRT_MAX || tmp < 0) {
        perror("Server Port Error:\n");
        exit(1);
    }
    server_addr.sin_port = htons((unsigned short) tmp);

    if (connect(client_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect failed.");
        exit(1);
    }

    if ((num_bytes = recv(client_fd, buf, MAX_CMD_STR, 0)) == -1) {
        perror("recv error.");
        exit(1);
    }
    buf[num_bytes] = '\0';
    printf("Message from server: %s\n", buf);

    bzero(buf, MAX_CMD_STR);
    FILE *file = fopen("./td.txt","r+");
    while (fgets(buf, MAX_CMD_STR, file) != NULL){
        send(client_fd, buf, strlen(buf), 0);
        num_bytes = recv(client_fd, buf, MAX_CMD_STR, 0);
        buf[num_bytes] = '\0';
        printf("Message from server: %s\n", buf);
    }

    close(client_fd);
    return 0;
}