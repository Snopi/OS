
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

#define BUF_SIZE 10000

int main(int argc, char **argv) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    bzero(&hints, sizeof hints);
    hints.ai_family = AF_INET; // Ipv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //bind
    int gaddrresp = getaddrinfo(NULL, argv[1], &hints, &result);
    
    int serv_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(serv_sock, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);
    
    listen(serv_sock, 100);
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
            struct buf_t *b = buf_new(BUF_SIZE);
            while(buf_fill(file, b, b->capacity) > 0) {
                buf_flush(fd, b, b->size);
            }
            buf_free(b);
            close(fd);
            close(file);
            return 0;
        }
    }  


    return 0;
}
