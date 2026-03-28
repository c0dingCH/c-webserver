#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <fstream>
int main() {
    const char* host = "0.0.0.0";   
    int port = 8888;                  

    // 1. 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        return -1;
    }

    // 2. 设置服务器地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, host, &servaddr.sin_addr);

    // 3. 连接服务器
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        close(sockfd);
        return -1;
    }
	  std::string request = "GET /  HTTP/1.1\r\n"
					  "Host: 127.0.0.1:1234\r\n"
					  "Connection: keep-alive\r\n"
					  "Cache-Control: max-age=0\r\n"
					  "sec-ch-ua: \"Google Chrome\";v=\"113\", \"Chromium\";v=\"113\", \"Not-A.Brand\";v=\"24\"\r\n"
					  "sec-ch-ua-mobile: ?0\r\n"
					  "sec-ch-ua-platform: \"Linux\"\r\n"
					  "Upgrade-Insecure-Requests: 1\r\n"
					  "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n"
					  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
					  "Sec-Fetch-Site: none\r\n"
					  "Sec-Fetch-Mode: navigate\r\n"
					  "Sec-Fetch-User: ?1\r\n"
					  "Sec-Fetch-Dest: document\r\n"
					  "Accept-Encoding: gzip, deflate, br\r\n"
					  "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,zh-TW;q=0.7\r\n"
					  "Cookie: username-127-0-0-1-8888=\"2|1:0|10:1681994652|23:username-127-0-0-1-8888|44:Yzg5ZjA1OGU0MWQ1NGNlMWI2MGQwYTFhMDAxYzY3YzU=|6d0b051e144fa862c61464acf2d14418d9ba26107549656a86d92e079ff033ea\"; _xsrf=2|dd035ca7|e419a1d40c38998f604fb6748dc79a10|168199465\r\n"
					  "\r\n"
            "username=123123123&pastatstatstatssword=12312312";
//      std::string request = "POST /upload HTTP/1.1\r\n"
//              "Host: localhost:8080\r\n"
//              "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary123456\r\n"
//              "Content-Length: 207\r\n"
//              "\r\n"
//              "------WebKitFormBoundary123456\r\n"
//              "Content-Disposition: form-data; name=\"file\"; filename=\"hello.txt\"\r\n"
//              "Content-Type: text/plain\r\n"
//              "\r\n"
//              "Hello, this is the file content.\r\n"
//              "It is written by hand.\r\n"
//              "------WebKitFormBoundary123456--\r\n";

    // 5. 发送请求
    send(sockfd, request.c_str(), request.size(), 0);

    // 6. 接收响应
    char buffer[4096];
    int n;
    while ((n = recv(sockfd, buffer, sizeof(buffer)-1, 0)) > 0) {
      buffer[n] = '\0';
      std::cout << buffer;
    }


    // 7. 关闭 socket
    close(sockfd);
    return 0;
}

