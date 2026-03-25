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
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0"); // 本地测试

    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "connect error\n";
        close(sockfd);
        return -1;
    }
    
    char buf[1024];

    while(1){
      std::string s;
      getline(std::cin,s);
      write(sockfd,s.c_str(),s.size());
    
      memset(buf,0,sizeof buf);
      read(sockfd, buf, sizeof buf);
      std::cout<<buf<<std::endl;
    }



    return 0;
}

