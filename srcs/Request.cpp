#include "../includes/Request.hpp"

RequestLine::RequestLine(void)
{
    method = std::string();
    path = std::string();
    httpVersion = std::string();
}

RequestLine::~RequestLine(void)
{

}

Request::Request(void)
{ 
    httpHeaders = std::map<std::string, std::string>();
}

bool Request::pathURIChecker(std::string& URI)
{
    for(std::string::iterator i = URI.begin(); i != URI.end(); i++)
    {
        if (!std::isprint(*i))
            return true;
    }
    return false;
}

int Request::valueChecker(std::vector<Server>& vec)
{
    if (!httpHeaders["Transfer-Encoding"].empty() && httpHeaders["Transfer-Encoding"] != "chunked")
        return 501;
    if (httpHeaders["Transfer-Encoding"].empty() && httpHeaders["Content-Length"].empty() && Component.method == "POST")
        return 400;
    if (Component.method != "POST" && Component.method != "DELETE" && Component.method != "GET")
        return 501;
    if (Component.method == "POST" && httpHeaders["Content-Length"].empty())
        return 400;
    if (pathURIChecker(Component.path))
        return 400;
    if (Component.path.size() > 2048)
        return 414;
    int cl = std::atof(httpHeaders["Content-Length"].c_str());
    for(std::vector<Server>::iterator i = vec.begin(); i != vec.end(); i++)
        if(sFd == i->fd && cl != i->maxBodySize)
            return 413;
    return 200;
}

void Request::generateCoresspondingErrorPage(void)
{

}

void Request::requestParser(std::string &request, std::vector<Server>& vec)
{
    // std::cout << request << std::endl;
    // std::cout << "===================================================\n";
    std::stringstream stream;
    std::string line;
    stream << request;
    std::getline(stream, line);
    line.erase(remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    std::vector<std::string> splitted = split(line);
    Component.method = *(splitted.begin());
    Component.path = *(splitted.begin() + 1);
    Component.httpVersion = *(splitted.begin() + 2); // if a segv happens it's probably caused by this line
    std::getline(stream, line);
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    while (!line.empty())
    {
        size_t i = line.find(':');
        std::pair<std::string, std::string> keyValue;
        keyValue.first = line.substr(0, i);
        keyValue.second = line.substr(i + 1, line.size() - i);
        httpHeaders.insert(keyValue);
        std::getline(stream, line);
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    }
    returnCode = valueChecker(vec);
}

void Request::printRequestComponents(void)
{
    std::cout << "method: " << Component.method << std::endl;
    std::cout << "path: " << Component.path << std::endl;
    std::cout << "httpVersion: " << Component.httpVersion << std::endl;
    std::cout << "---key | value---\n";
    for(std::map<std::string, std::string>::iterator i = httpHeaders.begin(); i != httpHeaders.end(); i++)
        std::cout << i->first << " | " << i->second << std::endl;
}


std::string Request::getRequest(void)
{
    std::ostringstream stream;
    int s = socket(AF_INET,SOCK_STREAM,0);
    if(s < 0)
        std::cout << "s " << strerror(errno) << std::endl, exit(1);
    struct sockaddr_in fam;
    int len = sizeof(fam);

    fam.sin_addr.s_addr = INADDR_ANY;
    fam.sin_port = htons(8080);
    fam.sin_family = AF_INET;
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&fam,sizeof(len));
    int x = bind(s, (struct sockaddr *)&fam, sizeof(fam));
    if (x < 0)
        std::cout << "x " << strerror(errno) << std::endl, exit(1);
    int d = listen(s, 10);
    if (d < 0)
        std::cout << "d " << strerror(errno) << std::endl, exit(1);
    while (true)
    {
        int ns = accept(s, (struct sockaddr *)&fam, (socklen_t*)&len);
        if (ns < 0)
            std::cout << "ns " << strerror(errno) << std::endl, exit(1);

        char buffer[1000000];
        int r = read(ns, buffer, 1000000);
        buffer[r] = 0;
        close(ns);
        return buffer;
    }
    std::string i = "failed to get the request\n";
    return i;
}

Request::~Request(void)
{

}
