#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pcsa_net.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define BUFSIZE 1024

int tmp;
char *port, *root;
typedef struct sockaddr SA;

const struct option options[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"root", required_argument, NULL, 'r'},
        {NULL, 0, 0, 0}
};

void serve_http(int connFd) {
    char buf[BUFSIZE];

    if (!read_line(connFd, buf, BUFSIZE)) 
        return ;

    printf("LOG: %s\n", buf);

    char method[BUFSIZE], uri[BUFSIZE], version[BUFSIZE];
    sscanf(buf, "%s %s %s", method, uri, version);

    while (read_line(connFd, buf, BUFSIZE) > 0) {
        if (strcmp(buf, "\r\n") == 0) break;
    }

    if (strcmp(method, "GET") == 0 &&
            strcmp(uri, "/") == 0) {
    }

}

int main(int argc, char *argv[]) 
{
    while ((tmp = getopt_long(argc, argv, "p:r:", options, NULL)) != -1) {
        switch(tmp){
            case 'p':
                port = optarg;
                break;
            case 'r':
                root = optarg;
                break;
        }
    }

    int listenFd = open_listenfd(port);

    for (;;) {
        struct sockaddr_storage clientAddr; // to store addr of the client
        socklen_t clientLen = sizeof(struct sockaddr_storage); // size of the above

        // ...gonna block until someone connects to our socket
        int connFd = accept(listenFd, (SA *) &clientAddr, &clientLen);
        if (connFd < 0)
        {
            printf("Failed to accept\n");
            continue;
        }

        // print the address of the incoming client
        char hostBuf[BUFSIZE], svcBuf[BUFSIZE];
        if (getnameinfo((SA *) &clientAddr, clientLen, hostBuf, BUFSIZE, svcBuf, BUFSIZE, 0) == 0) 
            printf("Connection from %s:%s\n", hostBuf, svcBuf); 
        else 
            printf("Connection from UNKNOWN.");
        
        serve_http(connFd); // service the client on this fd
        close(connFd);
    }
    return 0;
}