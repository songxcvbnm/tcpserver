#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
 
#define SEND_BUF_SIZE 512
#define RECV_BUF_SIZE 512
 
int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, peer_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&peer_addr, '0', sizeof(peer_addr));
    serv_addr.sin_family      = AF_INET;
    // serv_addr.sin_addr.s_addr = htonl("39.98.64.79");
    // serv_addr.sin_addr.s_addr = htonl("127.0.0.1");

    if(inet_aton("39.98.64.79", (struct in_addr*)&serv_addr.sin_addr.s_addr) == 0)
    {
        printf("ip is invalid\n");
        return;
    }

    serv_addr.sin_port        = htons(5000);
    int sleepCount = 60*5;
    if(argc == 2){
        sleepCount = atoi(argv[1]);
    }
    printf("heart beat every %d Sec\n",sleepCount);

    int listenfd = 0, connfd = 0;
    //创建socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        printf("Error. failed to create socket!\n");
        return -1;
    }
    //绑定socket
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error. failed to bind!\n");
        return -1;
    }
    //监听socket
    if (listen(listenfd, 10) < 0) {
        printf("Error. failed to listen!\n");
        return -1;
    }
 
    char sendBuff[SEND_BUF_SIZE] = {0};
    const char *src = "HEART-BEAT";
    strncpy(sendBuff, src, SEND_BUF_SIZE - 1);
    sendBuff[SEND_BUF_SIZE - 1] = '\0';
 
    int size = sizeof(struct sockaddr_in);
    while(1)
    {
        //等等客户端连接
        printf("waiting for client\n");
        connfd = accept(listenfd, (struct sockaddr*)&peer_addr, (socklen_t*)&size);
        if (connfd < 0) {
            printf("Error. accept failed!\n");
            return -1;
        }

        do{
            //连上之后，打印客户端的ip
            printf("has a clinet connet ip %s\n",inet_ntoa(peer_addr.sin_addr));
            //向客户端发送信息
            write(connfd, src, strlen(src));
            printf("write buffer success\n");
            int n = 0; char recvBuff[256];

            n = read(connfd, recvBuff, sizeof(recvBuff)-1);
            printf("read buff %d\n",n);
            if(n > 0){
                recvBuff[n] = 0;
                printf("buff = %s\n",recvBuff);                
            }
            
            if(n <= 0){
                printf("failed to read\n");
                close(connfd);
                connfd = -1;
            }else{
               sleep(60*2);
            }

    
        }while(connfd != -1);

        sleep(1);
     }
}
