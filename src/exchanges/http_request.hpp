#pragma once

#include <string>
#include <unordered_map>

class HttpRequest {
private:
    std::string method;
    std::string path;
    std::string body;
    std::unordered_map<std::string, std::string> headers;

public:
    HttpRequest(const std::string& method,
                const std::string& path,
                const std::string& body,
                const std::unordered_map<std::string, std::string>& headers);

    // Getters
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getBody() const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;
};
