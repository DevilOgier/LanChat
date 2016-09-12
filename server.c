#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>


#define PORT 10086
#define SIZE 32


int main(void)
{
    struct sockaddr_in addr;
    struct sockaddr_in peer_addr;

    int sockfd;
    int peer_fd;
    int ret;
    socklen_t len = sizeof(struct sockaddr_in);

    char buf[SIZE];

    int fdmax;
    fd_set readfds;
    struct timeval tmo;

   //1. 创建一个套接字
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        printf("socket failed...\n");
        goto err0;
    }

    //2. 初始化sockaddr_in结构体
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;  //IPv4
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("server_ip");

    //3. 绑定
    ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if (-1 == ret)
    {
        printf("bind failed....\n");
        goto err0;
    }

    //4. 监听
    ret = listen(sockfd, 10);
    if (-1 == ret)
    {
        printf("listen failed...\n");
        goto err0;
    }

    memset(&peer_addr, 0, sizeof(peer_addr));
    //5. 接受连接
    peer_fd = accept(sockfd, (struct sockaddr*)&peer_addr, &len);
    if (-1 == peer_fd)
    {
        printf("accept failed...\n");
        goto err0;
    }


    //服务器端发送消息给客户端
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
                printf("From client:  %s", buf);

            }

            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                //发
                //先发后收
                printf("please input:  ");
                fgets(buf, SIZE, stdin);
                ret = send(peer_fd, buf, strlen(buf), 0);
                printf("Server ---> send %d bytes\n", ret);
            }
        }
    }


    return 0;
err0:
    return -1;
}
