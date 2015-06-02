//getaddrinfo includes
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

#define BUF_SIZE 1024 
#define LISTEN_THRESHOLD 100
#define PERROR_AND_EXIT(a) { perror(a); exit(EXIT_FAILURE);}

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

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port1 port2\n", argv[0]);
        return 0;
    }

    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0)
        return -1;
 
    int serv_sock_1 = make_serv_sock(argv[1]);
    int serv_sock_2 = make_serv_sock(argv[2]);
       
    struct buf_t *b = buf_new(BUF_SIZE);//nifty buffer
    while (1) {
        struct sockaddr_in client;
        socklen_t sz1 = sizeof client;
        int cli1 = accept(serv_sock_1, (struct sockaddr*)&client, &sz1); 
   
        struct sockaddr_in client2;
        socklen_t sz2 = sizeof client2; 
        int cli2 = accept(serv_sock_2, (struct sockaddr*)&client2, &sz2); 
  
        int pid1 = fork();
        if (!pid1) {//from cli1 to cli2
            while (buf_fill(cli1, b, 1) > 0)
                buf_flush(cli2, b, b->size);
            return 0;  
        }

        int pid2 = fork();
        if (!pid2) {//from cli2 to cli1
            while (buf_fill(cli2, b, 1) > 0)
                buf_flush(cli1, b, b->size);
            return 0;  
        }

        close(cli1); //close sockets on server
        close(cli2); 
    }
    return 0;
}
