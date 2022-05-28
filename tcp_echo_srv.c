#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>

#define BACKLOG    5
#define MAX_CMD_STR 100

int listenfd, connfd;

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

void sig_int(int signo) {
    printf("[srv] SIGINT is coming!\n");
}

void sig_pipe(int signo) {
    printf("[srv] SIGPIPE is coming!\n");
}

void echo_req() {
    size_t num_bytes;
    char buf[MAX_CMD_STR];
    while (1) {
        size_t len;
        if (read_s(connfd, &len, sizeof(len)) == 0)
            return;
        num_bytes = read_s(connfd, buf, len);
        if (num_bytes == 0)
            return;
        buf[num_bytes] = '\0';
        printf("[echo_rqt] %s\n", buf);

        len = strnlen(buf, MAX_CMD_STR);
        write(connfd, &len, sizeof(len));
        write(connfd, buf, len);
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

    //register sigint sigpipe
    struct sigaction sigact_int, sigact_pipe;
    memset(&sigact_int, 0, sizeof(struct sigaction));
    memset(&sigact_pipe, 0, sizeof(struct sigaction));
    sigemptyset(&sigact_int.sa_mask);
    sigemptyset(&sigact_pipe.sa_mask);
    sigact_int.sa_handler = sig_int;
    sigact_pipe.sa_handler = sig_pipe;

    sigact_pipe.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sigact_int, NULL);
    sigaction(SIGPIPE, &sigact_pipe, NULL);


    //socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Create socket failed.");
        exit(-1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("[srv] server[%s:%s] is initializing!\n", argv[1], argv[2]);

    //bind
    if (bind(listenfd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("Bind error.");
        exit(-1);
    }

    //listen
    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen error.");
        exit(-1);
    }

    socklen_t sin_size = sizeof(struct sockaddr_in);
    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *) &client, &sin_size)) == -1 && errno == EINTR) {
            close(listenfd);
            printf("[srv] listenfd is closed!\n");
            printf("[srv] server is going to exit!\n");
            exit(0);
        }
        //client address
        char client_ip[20];
        if (inet_ntop(AF_INET, &client.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
            perror("get client addr error.");
            exit(-1);
        }
        //client port
        in_port_t client_port = ntohs(client.sin_port);
        printf("[srv] client[%s:%d] is accepted!\n", client_ip, client_port);

        echo_req();

        close(connfd);
        printf("[srv] connfd is closed!\n");
    }
    return 0;
}