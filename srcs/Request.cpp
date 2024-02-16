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

int Request::valueChecker(std::vector<Server>& srv)
{
    std::map<std::string, std::string>::iterator i = httpHeaders.find("Transfer-Encoding");
    if (i != httpHeaders.end() && i->second != "chunked")
        return 501;
    if (i == httpHeaders.end() && httpHeaders.find("Content-Length") == httpHeaders.end() && Component.method == "POST")
        return 400;
    if (Component.method != "POST" && Component.method != "DELETE" && Component.method != "GET")
        return 501;
    if (Component.method == "POST" && httpHeaders.find("Content-Length") == httpHeaders.end())
        return 400;
    if (pathURIChecker(Component.path))
        return 400;
    if (Component.path.size() > 2048)
        return 414;
    int cl = std::atof(httpHeaders.find("Content-Length")->second.c_str());
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
        if(sFd == i->fd && cl > i->maxBodySize)
            return 413;
    return 200;
}

std::string Request::errorPageMessage(void)
{
    if (returnCode == 400)
        return std::string("Bad Request");
    if (returnCode == 414)
        return std::string("Request URI Too Long");
    if (returnCode == 404)
        return std::string("Not Found");
    if (returnCode == 501)
        return std::string("Not Implemented");
    if (returnCode == 413)
        return std::string("Request Entity Too Large");
    return std::string();
}

std::pair<int, std::string> Request::generateCorrespondingErrorPage(void)
{
    if (returnCode != 200)
    {
        std::stringstream rCode;
        rCode << returnCode;
        std::string fileName = "/home/tyrant/webserv/ErrorPages/" + rCode.str() + ".html";
        std::ifstream content("/home/tyrant/webserv/ErrorPages/error_page.html");
        if (!content.is_open())
            std::cout << strerror(errno) << std::endl;
        std::stringstream file;
        file << content.rdbuf();
        std::string buffer = file.str();

        size_t pos = buffer.find("TEMPLATE-TEXT-HERE");
        while (pos != std::string::npos)
        {
            buffer.replace(pos, 18, errorPageMessage());
            pos = buffer.find("TEMPLATE-TEXT-HERE", pos + errorPageMessage().size());
        }
        std::ofstream result(fileName.c_str());
        result << buffer;
        return std::pair<int, std::string>(returnCode, fileName);
    }
    return std::pair<int, std::string>();
}

Location& Request::matchURIWithLocation(std::vector<Server>& srv)
{
    std::vector<Server>::iterator i;
    for (i = srv.begin(); i != srv.end(); i++)
    {
        if (sFd == i->fd)
        {
            if (!i->location.empty())
            {
                std::vector<Location>::iterator j;
                for (j = i->location.begin(); j != i->location.end(); j++)
                {
                    std::string absPath = Component.path.substr(0, Component.path.rfind('/') + 1);
                    if (absPath != "/" && absPath == j->path)
                        return *j;
                }
            }
        }
    }
    throw std::invalid_argument("no match");
}

void Request::requestParser(std::string &request, std::vector<Server>& srv)
{
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
    returnCode = valueChecker(srv);
    if (returnCode != 200)
        generateCorrespondingErrorPage();

    else
    {
        Location match;
        try
        {
            match = matchURIWithLocation(srv);
            // if (!match.ret.empty())
                // redirect to it;
            // else
                // send The corresponding response
                /*
                    http version | status code | message\r\n
                    Content-Type: <value>\r\n
                    Content-Length: <value>\r\n
                    \r\n
                    <body>
                */ 
        }
        catch(const std::exception& e)
        {
            std::string absPath = Component.path.substr(0, Component.path.rfind('/') + 1);
            if (absPath != "/")
                returnCode = 404;
            generateCorrespondingErrorPage();
        }
    }
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
