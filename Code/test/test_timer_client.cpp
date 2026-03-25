#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket error\n";
        return -1;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888); // 服务器端口
    servaddr.sin_addr.s_addr = inet_addr("192.168.72.130"); // 本地测试

    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "connect error\n";
        close(sockfd);
        return -1;
    }
    
    std::string s;
    getline(std::cin,s);
    write(sockfd,s.c_str(),s.size());
    

    int cnt = 3;
    while(cnt--){
      char buf[100]={0};
      read(sockfd,buf,sizeof buf);
      std::cout<<buf<<std::endl;
    }

    // 等待一会儿再关闭，方便服务端检测到断开

    return 0;
}

