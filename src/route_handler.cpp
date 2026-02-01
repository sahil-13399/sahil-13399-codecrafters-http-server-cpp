#include "route_handler.hpp"
#include "utils/string_utils.hpp"
#include <fstream>
#include<iostream>
//IMPLEMENTATIONS

HttpResponse RouteHandler::handleRoute(HttpRequest httpRequest, bool keep_alive, bool compress_body) {
    HttpResponse httpResponse;
    std::cout<<"Received Request in Handler"<<std::endl;
    if(httpRequest.getPath() == "/") {
        emptyPathHandler(httpRequest, httpResponse);
    } else if(httpRequest.getPath().substr(0, 6) == "/echo/") {
        echoPathHandler(httpRequest, httpResponse);
    } else if(httpRequest.getPath().substr(0, 11) == "/user-agent") {
        userAgentPathHandler(httpRequest, httpResponse);
    } else if(httpRequest.getPath().substr(0, 7) == "/files/") {
        filePathHandler(httpRequest, httpResponse);
    } else {
        failurePathHandler(httpRequest, httpResponse);
    }
    KeepAliveAdd(httpResponse, keep_alive);
    CompressBodyAdd(httpResponse, compress_body);

    return httpResponse;
}


void RouteHandler::emptyPathHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    httpResponse.setHttpStatus(HTTP_OK);
}

void RouteHandler::echoPathHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    int len = httpRequest.getPath().length() - 6;
    httpResponse.setHttpStatus(HTTP_OK);
    httpResponse.addHeader("Content-Type", "text/plain").addHeader("Content-Length", std::to_string(len));
    httpResponse.setBody(httpRequest.getPath().substr(6, len));
}

void RouteHandler::userAgentPathHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    httpResponse.setHttpStatus(HTTP_OK);
    std::string body = httpRequest.getHeaders().at("User-Agent:");
    httpResponse.setBody(body);
    httpResponse.addHeader("Content-Type", "text/plain").addHeader("Content-Length", std::to_string(body.length()));
}

void RouteHandler::filePathHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    if(httpRequest.getMethod() == "POST") {
        filePathPostHandler(httpRequest, httpResponse);
    } else {
         filePathGetHandler(httpRequest, httpResponse);
    }
}


void RouteHandler::filePathPostHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    std::string body = httpRequest.getBody();
    std::string filename = httpRequest.getPath().substr(7);
    std::string path = directory + filename;
    std::fstream file(path, std::ios::out);
    file<<body;
    httpResponse.setHttpStatus(HTTP_CREATED);
    file.close();
}


void RouteHandler::filePathGetHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    std::string filename = httpRequest.getPath().substr(7);
    std::string path = directory + filename;
    std::fstream file(path, std::ios::in);
    httpResponse.setHttpStatus(HTTP_OK);
    if (!file) {
        failurePathHandler(httpRequest, httpResponse);
        return;
    } else {
        std::string content = "";
        std::string temp;
        while(std::getline(file, temp)) {
            content+=temp;
        }
        httpResponse.setHttpStatus(HTTP_OK);
        httpResponse.setBody(content);
        httpResponse.addHeader("Content-Type", "application/octet-stream").addHeader("Content-Length", std::to_string(content.length()));
    }
}

void RouteHandler::failurePathHandler(const HttpRequest& httpRequest, HttpResponse& httpResponse) {
    httpResponse.setHttpStatus(HTTP_FAILURE);
}