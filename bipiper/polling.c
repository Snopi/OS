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


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port1 port2\n", argv[0]);
        return 0;
    }
    struct buf_t* bufs[MAX_CLIENTS * 2]; 
    struct pollfd pofds[MAX_CLIENTS * 2];
    struct pollfd *clients = pofds + 2; //for index matching
    nfds_t cli_cnt = 0;
    
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
                waiting_client = accept(pofds[0].fd, 0, 0);
                pofds[0].events = 0;
                pofds[1].events = POLLIN;
            } else if (pofds[1].revents & POLLIN) {
                int tmp_cli = accept(pofds[1].fd, 0, 0);
                
                bufs[cli_cnt] = buf_new(BUF_SIZE);
                bufs[cli_cnt + 1] = buf_new(BUF_SIZE);

                clients[cli_cnt].fd = waiting_client;
                clients[cli_cnt + 1].fd = tmp_cli;

                clients[cli_cnt].events = POLLIN;
                clients[cli_cnt + 1].events = POLLIN;

                cli_cnt += 2;

                pofds[0].events = POLLIN;
                pofds[1].events = 0;
            }
        }
        for (int i = 0; i < cli_cnt; i++) {
           if (clients[i].revents & POLLIN) {
               int old_size = bufs[i]->size;
               int new_size = buf_fill(clients[i].fd, bufs[i], 1);
               bufs[i]->buf[bufs[i]->size] = 0;
               printf("Filling buf %d --- from cli %d\n---%s---\n", i, i, bufs[i]->buf);
               if (bufs[i]->size == bufs[i]->capacity) {
                   clients[i].events &= ~POLLIN;
               }
               if (old_size == new_size) {
               }
               if (bufs[i]->size > 0) {
                   clients[i ^ 1].events |= POLLOUT;
               }
           }
           if (clients[i].revents & POLLOUT) {
               bufs[i]->buf[bufs[i ^ 1]->size] = 0;
               printf("Flushing buf %d --- to cli %d\n---%s---\n", i ^ 1, i,
                       bufs[i ^ 1]->buf);
               buf_flush(clients[i].fd, bufs[i ^ 1], 1);
               if (bufs[i ^ 1]->size == 0) {
                   clients[i].events &= ~POLLOUT;
               }
           }
        }
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


