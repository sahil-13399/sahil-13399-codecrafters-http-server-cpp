#include<vector>
#include<string>
#include "../exchanges/http_request.cpp"
#include "../exchanges/http_response.cpp"

std::vector<std::string> splitStringByStringDelimiter(const std::string& input, const std::string& delimiter);

HttpRequest getHttpRequest(std::string request);

void CompressBodyAdd(HttpResponse httpResponse, bool compress_body);

void KeepAliveAdd(HttpResponse httpResponse, bool keep_alive);