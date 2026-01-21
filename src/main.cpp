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


std::vector<std::string> split(const std::string& s, char delimiter);

void HandleSingleRequest(int client_fd, std::string directory, std::string http_request)
{
  const char *http_response = "HTTP/1.1 200 OK\r\n\r\n";
  const char *http_reject = "HTTP/1.1 404 Not Found\r\n\r\n";
  std::cout << "Client connected\n";
  std::vector<std::string> split_request = split(http_request, ' ');
  if (split_request[1] == "/")
  {
    send(client_fd, http_response, strlen(http_response), 0);
  }
  else if (split_request[1].substr(0, 6) == "/echo/")
  {
    // ECHO MESSAGE BACK in RESPONSE BODY
    int len = split_request[1].length() - 6;
    std::string message = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    message += std::to_string(len) + "\r\n\r\n" + split_request[1].substr(6, len);
    send(client_fd, message.c_str(), message.length(), 0);
  }
  else if (split_request[1].substr(0, 11) == "/user-agent")
  {
    int user_agent_index = http_request.find("User-Agent: ");
    int end_index = http_request.find("\r\n", user_agent_index);
    std::string body = http_request.substr(user_agent_index + strlen("User-Agent: "), end_index);
    std::string message = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(body.length() - 4) + "\r\n\r\n" + body;
    send(client_fd, message.c_str(), message.length(), 0);
  }
  else if (split_request[1].substr(0, 7) == "/files/")
  {

    if (split_request[0] == "POST")
    {
      // std::cout<<split_request[split_request.size() - 1]<<" "<<split_request[split_request.size() - 1].length()<<std::endl;
      int last_index = http_request.rfind("\r\n");
      std::string body = http_request.substr(last_index + strlen("\r\n"));
      std::string filename = split_request[1].substr(7);
      std::string path = directory + filename;
      std::fstream file(path, std::ios::out);
      file << body;
      const char *created_response = "HTTP/1.1 201 Created\r\n\r\n";
      std::cout << body << std::endl;
      send(client_fd, created_response, strlen(created_response), 0);
      file.close();
    }
    else
    {
      std::string filename = split_request[1].substr(7);
      std::string path = directory + filename;
      std::fstream file(path, std::ios::in);
      if (!file)
      {
        std::cerr << "Error opening the file for writing.";
        send(client_fd, http_reject, strlen(http_reject), 0);
      }
      else
      {
        std::string content = "";
        std::string temp;
        while (std::getline(file, temp))
        {
          content += temp;
        }
        std::string message = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
        send(client_fd, message.c_str(), message.length(), 0);
      }
      file.close();
    }
  }
  else
  {
    send(client_fd, http_reject, strlen(http_reject), 0);
  }
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void handle_request(int client_fd, std::string directory) {
    
    bool keep_alive = true;
    while(keep_alive) {
        char buffer[1024];
        std::cout << "Client connected\n";
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
        if (n <= 0)
        {
          keep_alive = false;
          return;
        }
        buffer[n] = '\0';
        std::string http_request(buffer);
          if(http_request.find("Connection: close") != std::string::npos) {
            keep_alive = false;
          }
        std::thread(HandleSingleRequest, client_fd, directory, http_request).detach();
    }
    close(client_fd);
}

int main(int argc, char **argv)
{
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
