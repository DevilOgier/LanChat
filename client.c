#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define PORT 10086
#define SIZE 128


//客户端
int main(void)
{
    int ret = 0;
    char buf[SIZE];
    char buf1[SIZE];

    int sockfd;
    struct sockaddr_in addr;

    //1. 创建套接字  tcp协议
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        printf("socket failed...\n");
        goto err0;
    }

    //2. 初始化struct sockaddr_in结构体
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("client_ip");

    //3. 连接到服务器
    ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (-1 == 0)
    {
        printf("connect failed....\n");
        goto err0;
    }

    printf("connect to Server successfully....\n");

    //4. 通讯

    while(1)
    {
        tmo.tv_sec = 3;
        tmo.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(peer_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        ret = select(peer_fd + 1, &readfds, NULL, NULL, &tmo);
        if (-1 == ret)
        {
            printf("select error...\n");
            goto err0;
        }
        else if (0 == ret)
        {
            printf("time out 3s....\n");
        }
        else
        {
            if (FD_ISSET(peer_fd, &readfds))
            {
                //收
                ret = recv(peer_fd, buf, SIZE, 0);
                buf[ret] = '\0';
                printf("From Server:  %s", buf);

            }

            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                //发
                //先发后收
                printf("please input:  ");
                fgets(buf, SIZE, stdin);
                ret = send(peer_fd, buf, strlen(buf), 0);
                printf("Client ---> send %d bytes\n", ret);
            }
        }
    }



    return 0;
err0:
    return -1;
}
