#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include "string_utils.hpp"
#include<unordered_map>
#include <zlib.h>

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
    int pos2 = pathMethodLine.rfind(" ");
    std::string path = pathMethodLine.substr(pos + 1, pos2 - (pos + 1));
    //Read headers
    std::unordered_map<std::string, std::string> headers;
    int index = 0;
    for(auto str : requestLine) {
        if(index == 0) {
            index++;
            continue;
        }
        if(str == "") {
            break;
        }
        index++;
        int pos = str.find(" ");
        headers.insert({str.substr(0, pos), str.substr(pos + 1)});
    }
    std::string body = "";
    while(index < requestLine.size()) {
        body += requestLine[index++];
    }
    std::cout<<"Body : "<<body;
    return HttpRequest(method, path, body, headers);
}

std::string compress_gzip(const std::string& str, int compressionlevel = Z_DEFAULT_COMPRESSION) {
    z_stream zs; // zlib's control structure
    memset(&zs, 0, sizeof(zs));

    // Initialize the zlib stream for compression with gzip format
    if (deflateInit2(&zs, compressionlevel, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw(std::runtime_error("deflateInit2 failed while compressing."));
    }

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // Retrieve the compressed bytes block-wise
    do {
        zs.next_out = (Bytef*)outbuffer;
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK); // Keep going until the stream is finished (Z_STREAM_END) or an error occurs

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) { // an error occurred
        throw(std::runtime_error("Exception during zlib compression: " + std::string(zs.msg)));
    }

    return outstring;
}

void CompressBodyAdd(HttpResponse& httpResponse, bool compress_body) {
  if(!compress_body) {
    return;
  }
  std::string body = httpResponse.getBody();
  int body_len = body.length();
  body = compress_gzip(body);
  httpResponse.setBody(body);
  httpResponse.addHeader("Content-Length", std::to_string(body.length()))
  .addHeader("Content-Encoding", "gzip");
}

void KeepAliveAdd(HttpResponse& httpResponse, bool keep_alive) {
  if(!keep_alive) {
    httpResponse.addHeader("Connection","close");
  }
}