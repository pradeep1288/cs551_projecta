
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define server_tcp_port  "21185" //The tcp port where the store's would connect to
#define server_udp_port1 "31185"
#define server_udp_port2 "32185"

#define store1_udp_recv1 "5185"
#define store1_udp_send1 "6185"
#define store1_udp_recv2 "7185"
#define store1_udp_send2 "8185"

#define store2_udp_recv1 "9185"
#define store2_udp_send1 "10185"
#define store2_udp_recv2 "11185"
#define store2_udp_send2 "12185"

#define store3_udp_recv1 "13185"
#define store3_udp_send1 "14185"
#define store3_udp_recv2 "15185"
#define store3_udp_send2 "16185"

#define store4_udp_recv1 "17185"
#define store4_udp_send1 "18185"
#define store4_udp_recv2 "19185"
#define store4_udp_send2 "20185"

#define MAX_CON 200 //max number of connection
