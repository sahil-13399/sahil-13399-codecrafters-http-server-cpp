#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <thread>
#include <zlib.h>
#include "utils/string_utils.hpp"
#include "exchanges/http_response.cpp"
#include "route_handler.hpp"

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void KeepAliveAdd(HttpResponse httpResponse, bool keep_alive) {
  if(!keep_alive) {
    httpResponse.addHeader("Connection","close");
  }
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

void CompressBody(HttpResponse httpResponse, bool compress_body) {
  if(!compress_body || httpResponse.getBody().empty()) {
    return;
  }
  httpResponse.addHeader("Content-Encoding","gzip");
  std::string body = httpResponse.getBody();
  int body_len = body.length();
  body = compress_gzip(body);
  httpResponse.setBody(body);
  httpResponse.addHeader("Content-Length", std::to_string(body.length()));
}

void handle_request(int client_fd, std::string directory) {
    bool keep_alive = true;
    bool compress_body = false;
    while(keep_alive) {
      char buffer[4096];
      std::string http_response = "HTTP/1.1 200 OK";
      std::string http_reject = "HTTP/1.1 404 Not Found";  
      std::string CRLF = "\r\n\r\n";  
      std::cout << "Client connected\n";

        ssize_t n = recv(client_fd, buffer, sizeof(buffer),0 );
        if(n <= 0) {
          break;
        }
        buffer[n] = '\0';
        std::string http_request(buffer);
        std::cout<<"Received request "<<http_request; 
        HttpRequest httpRequest = getHttpRequest(http_request);
        std::vector<std::string> split_request = split(http_request,' ');
        if(http_request.find("Connection: close") != std::string::npos) {
            std::cout<<"Setting to False";
            keep_alive = false;
        }
        if(http_request.find("Accept-Encoding: ") != std::string::npos && http_request.find("gzip") != std::string::npos) {
            std::cout<<"Setting compression to TRUE";
            compress_body = true;
        }
        RouteHandler routeHandler(httpRequest.getPath(), directory);
        HttpResponse httpResponse = routeHandler.handleRoute(httpRequest, keep_alive, compress_body);
        std::string finalMessage = httpResponse.build();
        send(client_fd, finalMessage.c_str(), finalMessage.length(), 0); 
    }
    close(client_fd);
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // TODO: Uncomment the code below to pass the first stage
  
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";

  while(true) {
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
      std::thread(handle_request, client_fd, argv[2]).detach();
  }
  close(server_fd);

  return 0;
}