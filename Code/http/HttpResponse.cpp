#include"HttpResponse.h"



HttpResponse::HttpResponse(bool close_connection):
  status_code_(HttpStatusCode::kUnknow), 
  close_connection_(close_connection)
  {}

HttpResponse::~HttpResponse(){}


void HttpResponse::SetStatusCode(HttpStatusCode status_code){
  status_code_ = status_code;
}

void HttpResponse::SetStatusMessage(const std::string & status_message){
  status_message_ = status_message;
}

void HttpResponse::SetCloseConnection(bool close_connection){
  close_connection_ = close_connection;
}

void HttpResponse::SetContentType(const std::string & content_type){
  headers_["Content-Type"] = content_type;
}

void HttpResponse::AddHeader(const std::string & key, const std::string &value){
  headers_[key] = value;
}

void HttpResponse::SetBody(const std::string & body){
  body_ = body;
  SetBodyLength(body_.size());
}
void HttpResponse::SetBodyLength(int body_length){
  body_length_ = body_length;
}
int HttpResponse::GetBodyLength(){
  return body_length_;
}



bool HttpResponse::IsInCloseConnection(){
  return close_connection_;
} 

std::string HttpResponse::GetMessage(){
  return GetBeforeBody() + body_ + "\r\n";

}

std::string HttpResponse::GetBeforeBody(){
  std::string message;
  message += ("HTTP/1.1 " + 
              std::to_string(status_code_) +
              " " + 
              status_message_ + "\r\n" 
  );
  
  if(close_connection_){
    message += "Connection: close\r\n";
  }
  else{
    if(body_type_ == HttpBodyType::kHtml){
      message += "Content-Length: " + std::to_string(body_.size()) + "\r\n";
    }
    else{
      message += "Content-Length: " + std::to_string(body_length_) + "\r\n";
    }

    message += "Connection: Keep-Alive\r\n";
  }

  for(const auto &header  : headers_){
    message += header.first + ":" + header.second + "\r\n";
  }

  message += "\r\n";

  return message;
} 


void HttpResponse::SetBodyType(HttpBodyType body_type){
  body_type_ = body_type;
}

HttpResponse::HttpBodyType HttpResponse::GetBodyType(){
  return body_type_;
}

void HttpResponse::SetFileFd(int filefd){
  filefd_ = filefd;
}

int HttpResponse::GetFileFd(){
  return filefd_;
}




