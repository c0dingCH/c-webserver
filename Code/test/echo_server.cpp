#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "TcpConnection.h"
#include <iostream>
#include <functional>
#include <memory>
int main(int argc, char *argv[]) {
  TcpServer *server = new TcpServer("0.0.0.0",8888);

  server->OnMessage([](const std::shared_ptr<TcpConnection> &conn){                        
    std::cout << "Message from client " << conn->GetFd() << " is: " << conn->GetReadBuffer()->PeekAllAsString() << std::endl;
    conn->SendInLoop(conn->GetReadBuffer() -> PeekAllAsString().data());
    //std::string msg(5e6, 'x');
    //conn->SendInLoop(msg.data(), msg.size());  
  });

  server->OnConnect([](const std::shared_ptr<TcpConnection> &conn){
    std::cout<< "client fd : " << conn->GetFd()<< "  connected "<< std::endl;
  
  });
  server->Start();

  delete server;
  return 0;
}
