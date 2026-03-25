#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "TcpConnection.h"
#include <iostream>
#include <functional>
#include <memory>
#include "TimeStamp.h"
int main(int argc, char *argv[]) {
  TcpServer *server = new TcpServer("192.168.72.130",8888);


  server->OnConnect([](const std::shared_ptr<TcpConnection> &conn){
    std::cout<<"client fd : "<<conn->GetFd()<<" connected"<<std::endl;
  });

  server->OnMessage([](const std::shared_ptr<TcpConnection> &conn){                        
    std::cout << "Message from client " << conn->GetFd() << " is: " << conn->GetReadBuffer()->ToStr() << std::endl;

    auto loop = conn->GetLoop();
    loop -> RunAt(TimeStamp::Now(),[conn](){
      std::string s = "are you ok?";
      conn->Send(s.c_str());
    });

  });
  server->Start();

  delete server;
  return 0;
}
