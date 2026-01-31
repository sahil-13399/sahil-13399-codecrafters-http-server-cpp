#include<sstream>
#include<iostream>
#include "string_utils.hpp"
#include<unordered_map>

std::vector<std::string> splitStringByStringDelimiter(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t startPos = 0;
    size_t endPos = 0;
    while ((endPos = input.find(delimiter, startPos)) != std::string::npos) {
        std::string token = input.substr(startPos, endPos - startPos);
        tokens.push_back(token);
        startPos = endPos + delimiter.length();
    }
    tokens.push_back(input.substr(startPos));
    
    return tokens;
}

HttpRequest getHttpRequest(std::string request) {
    std::vector<std::string> requestLine = splitStringByStringDelimiter(request, "\r\n");
    std::string pathMethodLine = requestLine[0];
    int pos = pathMethodLine.find(" ");
    std::string method = pathMethodLine.substr(0, pos);
    int pos2 = pathMethodLine.find(" ", pos);
    std::string path = pathMethodLine.substr(pos + 1, pos2 - (pos + 1));
    std::cout<<path<<" || "<<method;
    //Read headers
    std::unordered_map<std::string, std::string> headers;
    int index = 0;
    for(auto str : requestLine) {
        if(str == "") {
            break;
        }
        index++;
        int pos = str.find(" ");
        headers.insert({str.substr(0, pos), str.substr(pos + 1)});
        std::cout<<"Pair"<<str.substr(0, pos)<<" "<<str.substr(pos + 1);
    }
    std::string body = "";
    while(index < requestLine.size()) {
        body += requestLine[index];
    }
    std::cout<<"Body : "<<body;
    return HttpRequest(method, path, body, headers);

