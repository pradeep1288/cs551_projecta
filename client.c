#include "client.h"

int myclient (int server_tcp_port)
{
    int sockfd, rv;
    struct addrinfo hints, *servinfo, *p;
    char buf[1024], port_no[1024], data_to_send[1024];
    sprintf(port_no,"%d", server_tcp_port);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo("localhost", port_no, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    //get the dynamic tcp port number
    freeaddrinfo(servinfo); // all done with this structure
    recv(sockfd, buf, sizeof(buf), 0);
    printf("receiving noonce %s from manager now...\n", buf);
    sprintf(data_to_send,"%s %d",buf, getpid());
    send(sockfd, data_to_send, sizeof(data_to_send), 0);
    printf("sent %s to manager\n", data_to_send);
    close(sockfd);
    exit(0);
}