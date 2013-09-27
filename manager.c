#include "manager.h"

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

uint16_t getRandomPort()
{
    struct   sockaddr_in sin;
    int addrlen, sd_server;
    addrlen=sizeof(sin);
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr=inet_addr("127.0.0.1");
    sin.sin_port=htons(0);
    sd_server = socket(PF_INET, SOCK_STREAM, 0);
    bind(sd_server, (struct sockaddr *) &sin, sizeof(sin));
    getsockname(sd_server,(struct sockaddr*)&sin,&addrlen);
    return ntohs(sin.sin_port); 
}

Conf* read_config(char* path)
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
    Conf *conf;
    conf = read_config("foo.txt");
    printf("%d\n",getRandomPort());
    return 0;
}        
