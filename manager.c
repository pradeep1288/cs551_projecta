#include "manager.h"
#define MAX_CON 2000
typedef struct config_file_info {
    int stage_no;
    int num_of_nodes;
    long noonce;
}Conf;
char buf[1024];

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



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

int client (int server_tcp_port, int client_num)
{
    int sockfd, rv, client_port, addrlen;
    struct sockaddr_in sa;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    char buf[1024], port_no[1024], data_to_send[1024];
    sprintf(port_no,"%d", server_tcp_port);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    FILE *fp;
    fp = fopen(LOG_FILE_NAME, "a+");
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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    //get the dynamic tcp port number
    addrlen = sizeof(sa);
    getsockname(sockfd,(struct sockaddr*)&sa,&addrlen);
    client_port=ntohs(sa.sin_port);
    printf("Client %d: %d and %d\n",client_num, client_port, getpid());
    client_port=ntohs(sa.sin_port);
    fprintf(fp, "Client %d: %d\n",client_num, client_port);
    freeaddrinfo(servinfo); // all done with this structure
    recv(sockfd, buf, sizeof(buf), 0);
    printf("receiving noonce %s from manager now...\n", buf);
    sprintf(data_to_send,"client %d says: %s %d",client_num, buf, getpid());
    send(sockfd, data_to_send, sizeof(data_to_send), 0);
    printf("sent %s to manager\n", data_to_send);
    close(sockfd);
    fclose(fp);
}

int main(int argc, char const *argv[])
{
    int sockfd, new_fd, rv, yes = 1, port, addrlen, i=0;  // listen on sock_fd, new connection on new_fd
    Conf *conf;
    conf = read_config(argv[1]);
    FILE *fp;
    fp = fopen(LOG_FILE_NAME, "a+");    
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in serv_sin;
    char server_tcp_port[1024], data_to_send[1024];
    sprintf(server_tcp_port,"%i",0);
    char servers_ip[INET6_ADDRSTRLEN];
    pid_t pid;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char buf[1024];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = INADDR_ANY;
    sprintf(data_to_send,"%ld", conf->noonce);
    if ((rv = getaddrinfo("localhost", server_tcp_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

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
        //store the servers ip in servers_ip
        inet_ntop(p->ai_family, get_in_addr(p->ai_addr),
            servers_ip, sizeof servers_ip);
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    addrlen = sizeof(serv_sin);
    getsockname(sockfd,(struct sockaddr*)&serv_sin,&addrlen);
    port=ntohs(serv_sin.sin_port);

    //write to log file about the port
    printf("Manager port: %d\n",port);
    fprintf(fp, "manager port:%d\n",port);
    fclose(fp);
    freeaddrinfo(servinfo); // all done with this structure

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
            printf("%d\n",port );
            client(port,i+1);
            exit(0);
        }
    }

    if (listen(sockfd, MAX_CON) == -1) {
        perror("listen");
        exit(1);
    }

    while(1)
    {
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        send(new_fd,data_to_send, sizeof(data_to_send), 0);
        recv(new_fd, buf, sizeof(buf), 0);
        fopen(LOG_FILE_NAME, "a+");
        fprintf(fp, "%s\n",buf);
        fclose(fp);
        printf("Received from client: %s\n",buf);
    }
    fclose(fp);
    return 0;
}        
