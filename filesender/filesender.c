//getaddrinfo includes
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



#define BUF_SIZE 101
#define LISTEN_THRESHOLD 100
#define PERROR_AND_EXIT(a) { perror(a); exit(EXIT_FAILURE);}


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port file [another files]\n", argv[0]);
        return 0;
    }

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
    if (serv_sock == -1) {
        PERROR_AND_EXIT("Serv_socket"); 
    }
    
    if (bind(serv_sock, result->ai_addr, result->ai_addrlen)) {
        PERROR_AND_EXIT("bind"); 
    }
    freeaddrinfo(result);
    
    if (listen(serv_sock, LISTEN_THRESHOLD)) {
        PERROR_AND_EXIT("listen"); 
    }


    int fd;
    while(1) {
        int delta = rand() % (argc - 2); //random file
        struct sockaddr_in client;
        socklen_t sz = sizeof client;
        fd = accept(serv_sock, (struct sockaddr*)&client, &sz); 
        if (fork()) {
            close(fd);
        } else {
            close(serv_sock);
            printf("delta: %d\n", delta);
            int file = open(argv[2 + delta], O_RDONLY);
            if (file == -1) {
                PERROR_AND_EXIT("fileopen");
            }
            struct buf_t *b = buf_new(BUF_SIZE);
            while(buf_fill(file, b, 1) > 0) {
                buf_flush(fd, b, b->size);
            }
            buf_free(b);
            close(file);
            close(fd);
            exit(EXIT_SUCCESS); 
        }
    }  


    return 0;
}
