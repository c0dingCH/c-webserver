/*
http_server_explain:
  auto close connection
  simple login in
  ...  

*/
#include <iostream>
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "EventLoop.h"
#include "Logging.h"

#include <fcntl.h>
#include <dirent.h>
#include <thread>
#include <string>
#include <fstream>

#include <sys/stat.h>

std::string ReadFile(const char * file_path){
  std::ifstream is(file_path, std::ifstream::in);
  if(!is){
    LOG_ERROR <<"File open error !";
    return nullptr;
  }

  is.seekg(0,is.end);
  int file_len = is.tellg();
  is.seekg(0,is.beg);

  std::vector<char> buffer(file_len);
  is.read(buffer.data(), file_len);


  return std::string(buffer.data(), file_len);
}

std::vector<std::string> FindAllFiles(const char * path){
  std::vector<std::string> files(0);
  DIR * dir = ::opendir(path);
  if(!dir){
    LOG_ERROR << "Dir open error !";
    return files;
  }

  dirent * dir_entry{nullptr};

  while((dir_entry = readdir(dir)) != nullptr){
    std::string name = dir_entry->d_name;
    if(name == "." || name == "..")continue;
    files.emplace_back(std::move(name));
  }

  return files;
}

std::string BuildFileHtml(){
  std::string msg;
  std::vector<std::string>file_names = FindAllFiles("../static/files");
  
  if(file_names.size() == 0){
    msg = "<tr><td> no files </td></tr>";
  }
  else for(auto & name : file_names){
    msg += "<tr><td>" + name + "</td>" +
            "<td>" + "<br>" + 
            "<a href=\"/download/" + name + "\">下载</a>" + "<br>" + 
            "<a href=\"/delete/" + name + "\">删除</a>" + "<br>" + 
            "</td></tr>" + "<br>";
  }
  
  std::string tmp = "<!--filelist-->";
  std::string html = ReadFile("../static/fileserver.html");
  int it = html.find(tmp);
  
  if(it == -1){
    LOG_ERROR << "Can't find where the list to put";
  }
  else{
    html.replace(it,tmp.size(),msg);
  }

  return html;
}

bool RemoveFile(std::string & filename){
  if(::remove(("../static/files/" + filename).c_str()) != -1){
    return true; 
  }
  else{
    LOG_ERROR << "remove file error";
    return false;
  }
}

bool DownLoadFile(const std::string & filename, HttpResponse * response){ 
  int filefd = ::open(("../static/files/" + filename).c_str(), O_RDONLY);
  if(filefd == -1){
    LOG_ERROR << "OPEN FILE ERROR";
    return false;
  }
  
  struct stat file_state;
  fstat(filefd, &file_state);
 
  response->SetBodyType(HttpResponse::HttpBodyType::kFile);
  response->SetFileFd(filefd);
  response->SetBodyLength(file_state.st_size);
  
  return true;
}


void HttpResponseCallback(HttpRequest * request, HttpResponse *response)
{
  const auto method = request->GetMethod();
  if(method != HttpRequest::Method::kGet && method != HttpRequest::Method::kPost){
    response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
    response->SetStatusMessage("Bad Request");
    response->SetCloseConnection(true);
  }

  if(method == HttpRequest::Method::kGet){
    std::string url = request->GetUrl();
    if(url == "/"){     
      const std::string body = ReadFile("../static/files/index.html"); 
      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetBody(body);
      response->SetContentType("text/html");
    }
    else if(url == "/msg.html"){
      const std::string body = ReadFile("../static/files/msg.html"); 
      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetBody(body);
      response->SetContentType("text/html");
    }
    else if(url == "/cat.jpg"){
      const std::string body = ReadFile("../static/files/cat.jpg");
      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetBody(body);
      response->SetContentType("image/jpeg");
    }
    else if(url == "/fileserver"){
      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetBody(BuildFileHtml());
      response->SetContentType("text/html");
    }
    else if(url.substr(0,7) == "/delete"){
      std::string file_path = url.substr(8);
      response->SetContentType("text/html");
      if(RemoveFile(file_path)){
        response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        response->SetBody("remove successfully!");
      }
      else{
        response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
        response->SetBody("remove error !");
      }
    }
    else if(url.substr(0,9) == "/download"){
      if(DownLoadFile(url.substr(10), response)){
        response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        response->SetContentType("application/octet-stream");
        response->SetStatusMessage("Ok!");
      }
      else{
        response->SetStatusMessage("Moved Temporarily");
        response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
        response->SetBody("Downloading error");
        response->SetContentType("text/html");

      }
    }
    
    else{
      response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
      response->SetStatusMessage("Not Found");
      response->SetBody("Sorry Not Found\n");
      response->SetCloseConnection(true);
    }
  }
  else if(method == HttpRequest::Method::kPost){
    if(request->GetUrl().substr(0,7) == "/upload"){
      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetBody("Upload successfully");
      response->SetContentType("text/html");
    }
    else{
      const std::string body = request->GetBody();
    
      int it_user_begin = body.find("username=") + 9;
      int it_pass_begin = body.find("password=") + 9;
      int it_user_end = body.find('&', it_user_begin);
      int it_pass_end = it_pass_begin;

      int &it = it_pass_end;
      while(it < static_cast<int>(body.size()) && body[it] != ' ' && body[it] != '\r' && body[it] != '\n') it++;
    
      std::string username = body.substr(it_user_begin, it_user_end - it_user_begin);
      std::string password = body.substr(it_pass_begin, it_pass_end - it_pass_begin);

      if(username == "123123123" && password == "123123123"){
        response->SetBody("Login ok !");
      }
      else{
        response->SetBody("Login unsuccessfully !");
      }

      response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
      response->SetStatusMessage("OK");
      response->SetContentType("text/plain");
    }
  }
  return;
}

int main(int argc, char *argv[]){
  int port;
  if (argc <= 1)
  {
      port = 8888;
  }else if (argc == 2){
      port = atoi(argv[1]);
  }else{
      printf("error");
      exit(0);
  }
  int size = std::thread::hardware_concurrency();
  HttpServer *server = new HttpServer("0.0.0.0", port);
  server->SetHttpCallback(HttpResponseCallback);
  server->SetThreadNums(size);
  //server->SetAutoCloseConn(true);
  server->Start();
  
  //delete loop;
  //delete server;
  return 0;
}
