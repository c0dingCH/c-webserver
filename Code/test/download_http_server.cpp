#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

// 简单的 HTTP 下载函数
int download_file(const char *host, const char *path, const char *filename) {
    int sockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char request[1024];
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // 解析域名
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "错误: 无法解析主机 %s\n", host);
        return -1;
    }

    // 创建 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket 创建失败");
        return -1;
    }

    // 设置服务器地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888); // HTTP 默认端口
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("连接失败");
        close(sockfd);
        return -1;
    }

    // 构造 HTTP GET 请求
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             path, host);

    // 发送请求
    write(sockfd, request, strlen(request));

    // 确保 ../static 目录存在
    mkdir("../static", 0755);

    // 打开文件保存
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "../static/%s", filename);
    int fd = open(filepath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("无法创建文件");
        close(sockfd);
        return -1;
    }

    // 读取响应并写入文件
    int header_parsed = 0;
    while ((bytes_read = read(sockfd, buffer, BUFFER_SIZE)) > 0) {
        if (!header_parsed) {
            // 跳过 HTTP 响应头
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                header_parsed = 1;
                body += 4; // 跳过分隔符
                write(fd, body, bytes_read - (body - buffer));
            }
        } else {
            write(fd, buffer, bytes_read);
        }
    }

    close(fd);
    close(sockfd);

    printf("文件已下载并保存到: %s\n", filepath);
    return 0;
}

int main() {
    const char *host = "0.0.0.0";   // 目标主机
    const char *path = "/download/index.html";   // 文件路径
    const char *filename = "index.html"; // 保存文件名

    download_file(host, path, filename);
    return 0;
}

