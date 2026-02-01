#include<iostream>
#include<string>
#include <unordered_map>


class HttpResponse{
    private:
       std::string httpStatus;
       std::string body;
       std::unordered_map<std::string, std::string> headers;

    public:
        HttpResponse() = default;

        HttpResponse(const std::string& httpStatus,const  int status,const std::string& body,const std::unordered_map<std::string, std::string>& headers) : httpStatus(httpStatus), body(body), headers(headers) {}
        
        std::string build() const {
            std::string response = httpStatus + "\r\n";
            
            // Add headers
            for (const auto& header : headers) {
                response += header.first + ": " + header.second + "\r\n";
            }
            
            // Add Content-Length if not already present
            if (headers.find("Content-Length") == headers.end()) {
                response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
            }
            
            // End headers with empty line
            response += "\r\n";
            // Add body
            response += body;
            return response;
        }

        HttpResponse& addHeader(const std::string& key, const std::string& value) {
            headers[key] = value;
            return *this;
        }

        // Setters
        void setHttpStatus(const std::string& status) {
            httpStatus = status;
        }
        
        void setBody(const std::string& bodyContent) {
            body = bodyContent;
        }

        const std::string& getHttpStatus() const {
            return httpStatus;
        }
        
        const std::string& getBody() const {
            return body;
        }
        
        const std::unordered_map<std::string, std::string>& getHeaders() const {
            return headers;
        }
};