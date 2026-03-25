#pragma once
#include<string>
#include<map>

class HttpResponse{
public:
  enum HttpStatusCode{
    kUnknow = 1,
    k100Continue = 100,
    k200K = 200,
    k302K = 302,
    k400BadRequest = 400,
    k403Forbidden = 403,
    k404NotFound = 404,
    k500internalServerError = 500
  };
  
  enum HttpBodyType{
    kHtml,
    kFile
  };

  HttpResponse(bool close_connection);
  ~HttpResponse();

  void SetStatusCode(HttpStatusCode status_code);
  void SetStatusMessage(const std::string & message);
  void SetCloseConnection(bool close_connection);
  
  void SetContentType(const std::string &content_type);
  void AddHeader(const std::string& key, const std::string &value);

  void SetBody(const std::string & body);
  void SetBodyLength(int len);
  int GetBodyLength();

  std::string GetMessage();
  std::string GetBeforeBody();

  bool IsInCloseConnection();

  void SetBodyType(HttpBodyType body_type);
  HttpBodyType GetBodyType();
  void SetFileFd(int filefd);
  int GetFileFd();

private:
  std::map<std::string,std::string>headers_;

  HttpStatusCode status_code_;
  std::string status_message_;
  std::string body_;
  int body_length_{0};
  bool close_connection_{false};

  HttpBodyType body_type_{HttpBodyType::kHtml};
  int filefd_{-1};
};
