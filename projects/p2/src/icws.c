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

#define BUFSIZE 8192

int tmp;
char *port, *root, *path = NULL;
typedef struct sockaddr SA;

const struct option options[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"root", required_argument, NULL, 'r'},
        {NULL, 0, 0, 0}
};

// https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void serve_http(int connFd) {
    char buf[BUFSIZE];
    char buf2[BUFSIZE];
    char method[BUFSIZE];
    char httpVersion[BUFSIZE];
    char *ext;
    char *file_ext;
    char *path;

    Request *request = parse(buf, BUFSIZE, connFd);

    strcpy(method, request->http_method);
    strcpy(httpVersion, request->http_version);

    ext = get_filename_ext(NULL);

    while (read_line(connFd, buf, BUFSIZE) > 0) {
        if (strcmp(buf, "\r\n") == 0) break;
    }

    // Get MIME TYPE
    if ((strcmp(ext, "html") == 0) || strcmp(ext, "htm") == 0) {
        strcpy(ext, "text,html");
    } else if (strcmp(ext, "css") == 0) {
        strcpy(ext, "text/css");
    } else if (strcmp(ext, "txt") == 0) {
        strcpy(ext, "text/plain");
    } else if (strcmp(ext, "js") == 0) {
        strcpy(ext, "text/javascript");
    } else if (strcmp(ext, "jpg") == 0) {
        strcpy(ext, "image/jpg");
    } else if (strcmp(ext, "png") == 0) {
        strcpy(ext, "image/png");
    } else if (strcmp(ext, "gif") == 0) {
        strcpy(ext, "image/gif");
    }


    // NULL request
    if (request == NULL) {
        sprintf(buf2,
        "HTTP1.1 400 Bad Request\r\n"
        "Server : ICWS\r\n"
        "Connection : close\r\n"
        "Content-Type : %s\r\n", NULL);
        write_all(connFd, buf, strlen(buf));
        return;
    } 

    // HTTP version
    if (strcasecmp(httpVersion, "HTTP1.1") != 0) {
        sprintf(buf2,
        "HTTP1.1 505 Http Version Not Support\r\n"
        "Server : ICWS\r\n"
        "Connection : close\r\n"
        "Content-Type : %s\r\n", NULL);
        write_all(connFd, buf, strlen(buf));
        return;
    }

    // GET and HEAD method
    if (strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) {
        sprintf(buf, "HTTP/1.1 200 OK\r\n"
    "Server: ICWS\r\n"
    "Connection: close\r\n"
    "Content-type: text/html\r\n\r\n", NULL);
    write_all(connFd, buf, strlen(buf));
        if (strcmp(method, "GET") == 0) {
            printf("GET Method");
            return;
        } else if (strcmp(method, "HEAD") == 0) {
            printf("HEAD Method");
            return;
        } else {
            sprintf(buf, "HTTP/1.1 501 Unsupported Version\r\n"
            "Server: ICWS\r\n"
            "Connection: close\r\n");
        }
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

    // Still parsing fail ****
}