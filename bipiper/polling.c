#define _GNU_SOURCE
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> // RDONLY
#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>

#define BUF_SIZE 2048
#define LISTEN_THRESHOLD 100
#define PERROR_AND_EXIT(a) { perror(a); exit(EXIT_FAILURE);}
#define MAX_CLIENTS 127

int make_serv_sock(char *port);

void swap_pollfds(struct pollfd *a, struct pollfd *b) {
    struct pollfd c = *a;
    *a = *b;
    *b = c;
}

void swap_bufs(struct buf_t **a, struct buf_t **b) {
    struct buf_t *c = *a;
    *a = *b;
    *b = c;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port1 port2\n", argv[0]);
        return 0;
    }
    signal(SIGPIPE, SIG_IGN);
    struct buf_t* bufs[MAX_CLIENTS * 2];
    struct pollfd pofds[MAX_CLIENTS * 2];
    struct pollfd *clients = pofds + 2; //for index matching
    nfds_t cli_cnt = 0;
    char can_write[MAX_CLIENTS * 2]; //number of closed ends
    char can_read[MAX_CLIENTS * 2];
    pofds[0].fd = make_serv_sock(argv[1]);
    pofds[0].events = POLLIN;

    pofds[1].fd = make_serv_sock(argv[2]);
    pofds[1].events = 0;

    int waiting_client;

    while (1) {
        int p_resp = poll(pofds, cli_cnt + 2, -1);
        if (p_resp < 0) {
            if (errno == EINTR) {
                continue;
            }
            PERROR_AND_EXIT("Poll");
        }
        if (cli_cnt < MAX_CLIENTS * 2) {
            if (pofds[0].revents & POLLIN) {
                waiting_client = accept4(pofds[0].fd, 0, 0, SOCK_NONBLOCK);
                pofds[0].events = 0;
                pofds[1].events = POLLIN;
            } else if (pofds[1].revents & POLLIN) {
                int tmp_cli = accept4(pofds[1].fd, 0, 0, SOCK_NONBLOCK);

                bufs[cli_cnt] = buf_new(BUF_SIZE);
                bufs[cli_cnt + 1] = buf_new(BUF_SIZE);

                clients[cli_cnt].fd = waiting_client;
                clients[cli_cnt + 1].fd = tmp_cli;

                clients[cli_cnt].events = POLLIN;
                clients[cli_cnt + 1].events = POLLIN;

                can_write[cli_cnt] = can_read[cli_cnt] = 1;
                can_write[cli_cnt + 1] = can_read[cli_cnt + 1] = 1;

                cli_cnt += 2;

                pofds[0].events = POLLIN;
                pofds[1].events = 0;
            }
        }
        printf("-----------------------------------\n");
        for (int i = 0; i < cli_cnt; i++)
            printf("B. i: %d, fd: %d, can_read: %d, can_write: %d\n", i, clients[i].fd, (int)can_read[i], (int)can_write[i]);
        for (int i = 0; i < cli_cnt; i++) {
            if (clients[i].revents & POLLHUP) {
                clients[i].revents = 0;
                close(clients[i].fd);
                clients[i].fd = -1;
                can_read[i] = 0;
                can_write[i] = 0;
                can_read[i ^ 1] = 0;
                if (bufs[i]->size == 0) {
                    can_write[i ^ 1] = 0;
                }
            }
            if (clients[i].revents & POLLIN) {
                int old_size = bufs[i]->size;
                int new_size = buf_fill(clients[i].fd, bufs[i], 1);
                if (new_size >= 0) {
                    if (bufs[i]->size == bufs[i]->capacity) {
                        clients[i].events &= ~POLLIN;
                    }
                    if (old_size == new_size) { //EOF
                        clients[i].events &= ~POLLIN;
                        can_read[i] = 0;
                        if (!can_write[i ^ 1]) {
                            bufs[i]->size = 0;
                        }
                    }
                    if (bufs[i]->size > 0) {
                        clients[i ^ 1].events |= POLLOUT;
                    }
                } else if (errno != EINTR && errno != EAGAIN) {
                    can_read[i] = 0;
                    clients[i].events &= ~POLLIN;
                }
            }
            if (clients[i].revents & POLLOUT) {
                int flush_res = buf_flush(clients[i].fd, bufs[i ^ 1], 1);
                if (flush_res == -1) {
                    if (errno == EINTR || errno == EAGAIN) {
                    } else {
                        bufs[i ^ 1]->size = 0; // no more writes
                        can_write[i] = 0;
                        can_read[i ^ 1] = 0; //don't read
                        clients[i ^ 1].revents &= ~POLLIN;
                        clients[i ^ 1].events &= ~POLLIN;
                    }
                }
                if (bufs[i ^ 1]->size == 0) {
                    clients[i].events &= ~POLLOUT;
                    if (!can_read[i ^ 1]) {
                        can_write[i] = 0;
                    }
                }
            }
            if (!can_read[i] && !can_write[i] && !can_read[i ^ 1] && !can_write[i ^ 1]) {
                swap_bufs(bufs + cli_cnt - 2, bufs + i);
                swap_bufs(bufs + cli_cnt - 1, bufs + (i ^ 1));

                can_write[i] = can_write[cli_cnt - 2];
                can_write[i ^ 1] = can_write[cli_cnt - 1];

                can_read[i] = can_read[cli_cnt - 2];
                can_read[i ^ 1] = can_read[cli_cnt - 1];

                swap_pollfds(clients + cli_cnt - 2, clients + i);
                swap_pollfds(clients + cli_cnt - 1, clients + (i ^ 1));

                close(clients[cli_cnt - 1].fd);
                close(clients[cli_cnt - 2].fd);

                buf_free(bufs[cli_cnt - 1]);
                buf_free(bufs[cli_cnt - 2]);

                cli_cnt -= 2;
                i = (i & ~1) - 1;
            }
        }

        printf("-----------------------------------\n");
        for (int i = 0; i < cli_cnt; i++)
            printf("A. i: %d, fd: %d, can_read: %d, can_write: %d\n", i, clients[i].fd, (int)can_read[i], (int)can_write[i]);
    }

    return 0;
}


int make_serv_sock(char *port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    bzero(&hints, sizeof hints);

    hints.ai_family = AF_INET; // Ipv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //bind
    hints.ai_protocol = IPPROTO_TCP;

    int ret_code = getaddrinfo(NULL, port, &hints, &result);
    if (ret_code) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret_code));
        exit(EXIT_FAILURE);
    }

    int serv_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serv_sock == -1)
        PERROR_AND_EXIT("Serv_socket");

    int one = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1)
        PERROR_AND_EXIT("setsockopt");

    if (bind(serv_sock, result->ai_addr, result->ai_addrlen))
        PERROR_AND_EXIT("bind");

    freeaddrinfo(result);

    if (listen(serv_sock, LISTEN_THRESHOLD))
        PERROR_AND_EXIT("listen");

    return serv_sock;
}


