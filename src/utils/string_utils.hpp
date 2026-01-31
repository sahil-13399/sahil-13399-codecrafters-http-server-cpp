#include<vector>
#include<string>
#include "../exchanges/http_request.cpp"

std::vector<std::string> splitStringByStringDelimiter(const std::string& input, const std::string& delimiter);

HttpRequest getHttpRequest(std::string request);