#include<iostream>
#include<string>
#include <unordered_map>
/*
GET /index.html HTTP/1.1\r\nHost: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: *\r\n\r\n
*/
class HttpRequest{
    private:
       std::string method;
       std::string path;
       std::string body;
       std::unordered_map<std::string, std::string> headers;

    public:
        HttpRequest(std::string& method, std::string& path, std::string& body, std::unordered_map<std::string, std::string>& headers) : method(method), path(path), body(body), headers(headers) {}

        // Getters
        const std::string& getMethod() const {
            return method;
        }
        
        const std::string& getPath() const {
            return path;
        }
        
        const std::string& getBody() const {
            return body;
        }
        
        const std::unordered_map<std::string, std::string>& getHeaders() const {
            return headers;
        }
        
};