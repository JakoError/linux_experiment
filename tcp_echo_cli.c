#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>

#define MAX_CMD_STR 100

int connfd;

size_t read_s(int fd, void *buf, size_t nbytes) {
    size_t read_n = 0, this_read;
    while (read_n != nbytes) {
        this_read = read(fd, buf, nbytes - read_n);
        if (this_read <= 0)
            return this_read;
        read_n += this_read;
    }
    return read_n;
}

void echo_rqt() {
    char buf[MAX_CMD_STR];
    while (fgets(buf, MAX_CMD_STR, stdin) != NULL) {
        char *p = strchr(buf, '\n');
        if (p)
            *p = '\0';
        if (strncmp(buf, "exit", 4) == 0)
            return;

        size_t len = strnlen(buf, MAX_CMD_STR);
        write(connfd, &len, sizeof(len));
        write(connfd, buf, len);

        bzero(buf, MAX_CMD_STR);

        read_s(connfd, &len, sizeof(len));
        read_s(connfd, buf, len);

        buf[strlen(buf)] = '\0';
        printf("[echo_rep] %s\n", buf);
    }
}

int main(int argc, char **argv) {
    struct sockaddr_in server, client;

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;

    //get ip
    if (inet_pton(AF_INET, argv[1], &server.sin_addr) == 0) {
        perror("Server IP Address Error:\n");
        exit(1);
    }

    //get port
    long tmp = strtol(argv[2], NULL, 10);
    if (tmp > USHRT_MAX || tmp < 0) {
        perror("Server Port Error:\n");
        exit(1);
    }
    server.sin_port = htons((unsigned short) tmp);


    //socket
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Create socket failed.");
        exit(1);
    }

    //connect
    if (connect(connfd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("Connect failed.");
        exit(1);
    }
    printf("[cli] server[%s:%s] is connected!\n", argv[1], argv[2]);

    //read stdin and send
    echo_rqt();

    close(connfd);
    printf("[cli] connfd is closed!\n");
    printf("[cli] client is going to exiting!\n");
    return 0;
}