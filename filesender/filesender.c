#define _GNU_SOURCE
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
#include <signal.h>


#define BUF_SIZE 1024 
#define LISTEN_THRESHOLD 100
#define PERROR_AND_EXIT(a) { perror(a); exit(EXIT_FAILURE);}


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port file\n", argv[0]);
        return 0;
    }
    
    struct sigaction sa; //Don't wait for children
    bzero(&sa, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0)
        return -1;
 
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    bzero(&hints, sizeof hints);
    
    hints.ai_family = AF_INET; // Ipv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //bind
    hints.ai_protocol = IPPROTO_TCP;

    int ret_code = getaddrinfo(NULL, argv[1], &hints, &result);
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
    //we can use SOMAXCONN here instead of LISTEN_THRESHOLD
    if (listen(serv_sock, LISTEN_THRESHOLD)) 
        PERROR_AND_EXIT("listen"); 

    int fd;
    while(1) {
        struct sockaddr_in client;
        socklen_t sz = sizeof client;
        fd = accept(serv_sock, (struct sockaddr*)&client, &sz); 
        
        if (fork()) {
            close(fd);
        } else {
            close(serv_sock);
            int file = open(argv[2], O_RDONLY);
            if (file == -1) {
                PERROR_AND_EXIT("fileopen");
            }
            struct buf_t *b = buf_new(BUF_SIZE);
            int success = 1;
            while(buf_fill(file, b, 1) > 0) 
                buf_flush(fd, b, b->size); //if something goes wrong
               // it will recieve SIGPIPE and die
            buf_free(b);//unnecessary actions..
            close(file);
            close(fd);
            exit(EXIT_SUCCESS); 
        }
    }  

    return 0;
}
