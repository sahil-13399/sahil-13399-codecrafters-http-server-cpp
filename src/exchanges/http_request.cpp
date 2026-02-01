#include "http_request.hpp"
#include <string>
#include <unordered_map>

HttpRequest::HttpRequest(const std::string& method,
                         const std::string& path,
                         const std::string& body,
                         const std::unordered_map<std::string, std::string>& headers)
    : method(method), path(path), body(body), headers(headers) {}

const std::string& HttpRequest::getMethod() const {
    return method;
}

const std::string& HttpRequest::getPath() const {
    return path;
}

const std::string& HttpRequest::getBody() const {
    return body;
}

const std::unordered_map<std::string, std::string>& HttpRequest::getHeaders() const {
    return headers;
}
