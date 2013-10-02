#include "client.h"

typedef struct config_file_info {
    int stage_no;
    int num_of_nodes;
    long noonce;
}Conf;
char buf[1024];

Conf* read_config(const char* path)
{
    Conf *conf = (Conf* )malloc(sizeof(Conf));
    FILE *fp;
    fp = fopen(path, "r");
    char temp[1024];
    int no_of_lines = 0;
    while (fp!=NULL)
    {
        fgets(buf, sizeof(buf), fp);
        if (buf[0]== '#')
        {
            no_of_lines++;
            continue;
        }
        else if (no_of_lines == 1)
        {
            sscanf(buf, "%s %d", temp, &(conf->stage_no));
            no_of_lines++;
        }        
        else if (no_of_lines == 2)
        {
            sscanf(buf, "%s %d",temp , &(conf->num_of_nodes));
            no_of_lines++;
        }
        else if (no_of_lines == 3)
        {
            sscanf(buf, "%s %ld", temp, &(conf->noonce));
            no_of_lines++;
        }
        else 
        {
            break;
        }
    }
    return conf;
}

int main(int argc, char const *argv[])
{
    int sockfd, new_fd, rv, yes = 1, port, addrlen, i=0,j=0;  // listen on sock_fd, new connection on new_fd
    Conf *conf;
    conf = read_config(argv[1]);
    FILE *fp;
    char LOG_FILE_NAME[1024];
    sprintf(LOG_FILE_NAME, "Stage%d.manager.out", conf->stage_no);
    fp = fopen(LOG_FILE_NAME, "a+");
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in serv_sin;
    char server_tcp_port[1024], data_to_send[1024],data_to_write[1024];
    sprintf(server_tcp_port,"%i",0);
    pid_t pid;
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    char buf[1024];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = INADDR_ANY;
    sprintf(data_to_send,"%ld\n", conf->noonce);
    if ((rv = getaddrinfo("localhost", server_tcp_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //line 76 - 96 has been adapted from http://beej.us/guide/bgnet/examples/server.c
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    addrlen = sizeof(serv_sin);
    getsockname(sockfd,(struct sockaddr*)&serv_sin,(socklen_t*)&addrlen);
    port=ntohs(serv_sin.sin_port);

    //write to log file about the port
    printf("Manager port: %d\n",port);
    fprintf(fp, "manager port:%d\n",port);
    fclose(fp);
    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, MAX_CON) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf("Forking clients now....\n");
    for (i=0;i<conf->num_of_nodes;i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
            exit(1);
        }
        if (pid == 0)  
        {
            /* This is the client process */
            close(sockfd);
            myclient(port);
            exit(0);
        }
    }

    for(j=0;j<conf->num_of_nodes;j++)
    {
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        printf("Client port:%d\n",ntohs(their_addr.sin_port));
        if (new_fd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        send(new_fd,data_to_send, sizeof(data_to_send), 0);
        recv(new_fd, buf, sizeof(buf), 0);
        fp = fopen(LOG_FILE_NAME, "a+");
        sprintf(data_to_write,"Client %d says: %s",j+1, buf);
        fprintf(fp, "Client %d: %d\n",j+1, ntohs(their_addr.sin_port));
        fprintf(fp, "%s\n",data_to_write);
        fclose(fp);
        printf("Received from client: %s\n",buf);
    }
    return 0;
}
