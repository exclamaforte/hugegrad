#include "Poco/Net/HTTPRequest.h"
#include "Poco/URI.h"
#include <fmt/core.h>

int main()
{
    Poco::URI uri("http://pocoproject.org/images/front_banner.jpg");
    std::string path(uri.getPathAndQuery());
    fmt::print("hello world: {}\n", path);
}
