#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define BACKLOG    5
#define MAX_CMD_STR 100

const char *HEAD = "[echo_rqt] ";

int main(int argc, char **argv) {
    int listen_fd, connect_fd;
    struct sockaddr_in server, client;
    unsigned int sin_size;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Create socket failed.");
        exit(-1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("Bind error.");
        exit(-1);
    }
    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen error.");
        exit(-1);
    }

    sin_size = sizeof(struct sockaddr_in);

    char buf[MAX_CMD_STR + strlen(HEAD)];
    for (int i = 0; i < strlen(HEAD); ++i)
        buf[i] = HEAD[i];
    int buf_start = strlen(HEAD);
    while (1) {
        if ((connect_fd = accept(listen_fd, (struct sockaddr *) &client, &sin_size)) == -1) {
            perror("accept error.");
            exit(-1);
        }
        printf("You get a connection from %s\n", inet_ntoa(client.sin_addr));
        send(connect_fd, "Welcome to my server.\n", 22, 0);
        long num_bytes;
        do {
            num_bytes = recv(connect_fd, buf + buf_start, MAX_CMD_STR, 0);
            buf[buf_start+num_bytes] = '\0';
            send(connect_fd, buf, strlen(buf), 0);
        } while (num_bytes != 0);

        close(connect_fd);
    }

    close(listen_fd);
    return 0;
}