#include<string>
#include<vector>
#include "exchanges/http_request.cpp"
#include "exchanges/http_response.cpp"
/*
Design plan
1. class RouteHandler stores path, client_fd, directory, keep_alive, constexpr of success and fail strings
2. Then has a function handleRoute
3. Then internally dispatches to the correct function
4. emptyPath, failurePath, userAgentPath, filePathHandler, filePathGet, filePathPost

*/

class RouteHandler {
    private:
        std::string path;
        std::string directory;
        
        const std::string HTTP_OK = "HTTP/1.1 200 OK";
        const std::string HTTP_CREATED = "HTTP/1.1 201 Created";
        const std::string HTTP_FAILURE = "HTTP/1.1 404 Not Found";

        void emptyPathHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void echoPathHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void failurePathHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void userAgentPathHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void filePathHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void filePathGetHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);
        void filePathPostHandler(const HttpRequest& httpRequest, HttpResponse& HttpResponse);

    public:
        //Constructors
        RouteHandler() = default;
        RouteHandler(std::string path, std::string directory) : path(path), directory(directory) {}
        
        HttpResponse handleRoute(HttpRequest httpRequest, bool keep_alive, bool compress_body);
};