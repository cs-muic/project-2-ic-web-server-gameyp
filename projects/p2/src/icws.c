#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pcsa_net.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "parse.h"

#define BUFSIZE 1024

int tmp;
char *port, *root, *path = NULL;
typedef struct sockaddr SA;

const struct option options[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"root", required_argument, NULL, 'r'},
        {NULL, 0, 0, 0}
};

void serve_http(int connFd) {
    char buf[BUFSIZE];
    char buf2[BUFSIZE];

    Request *request = parse(buf, BUFSIZE, connFd);

    struct stat sb;
    stat(path, &sb);
    char *lastMod = ctime(&sb.st_mtime);
    lastMod[strcspn(lastMod, "\n")] = 0;

    if (!read_line(connFd, buf, BUFSIZE)) 
        return ;

    while (read_line(connFd, buf, BUFSIZE) > 0) {
        if (strcmp(buf, "\r\n") == 0) break;
    }

    if (request == NULL) {
        sprintf(buf2,
        "HTTP/1.1 400 Bad Request\r\n"
        "Date : %s\r\n"
        "Server : ICWS\r\n"
        "Connection : close\r\n"
        "Content-Type : %s\r\n"
        "Content-Length : %lu\r\n"
        "Last-Modified : %s\r\n", );
    } 

    if (strcasecmp)


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