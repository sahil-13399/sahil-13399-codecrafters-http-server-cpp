#pragma once

#include <string>
#include <unordered_map>

class HttpResponse {
private:
    std::string httpStatus;
    std::string body;
    std::unordered_map<std::string, std::string> headers;

public:
    HttpResponse() = default;

    HttpResponse(const std::string& httpStatus,
                 int /*status*/,   // keep if you’ll use it later
                 const std::string& body,
                 const std::unordered_map<std::string, std::string>& headers);

    std::string build() const;

    HttpResponse& addHeader(const std::string& key, const std::string& value);

    // Setters
    void setHttpStatus(const std::string& status);
    void setBody(const std::string& bodyContent);

    // Getters
    const std::string& getHttpStatus() const;
    const std::string& getBody() const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;
};
