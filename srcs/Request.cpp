#include "../includes/Response.hpp"

RequestLine::RequestLine(void)
{
    method = std::string();
    path = std::string();
    httpVersion = std::string();
}

RequestLine::RequestLine(const RequestLine& other)
{
    *this = other;
}

const RequestLine& RequestLine::operator=(const RequestLine& other)
{
    method = other.method;
    path = other.path;
    httpVersion = other.httpVersion;
    return *this;
}

RequestLine::~RequestLine(void)
{

}

Request::Request(void)
{
    Component = RequestLine();
    httpHeaders = std::map<std::string, std::string>();
    returnCode = -1;
    sFd = -1;

}

Request::Request(const Request& other)
{
    *this = other;
}

const Request& Request::operator=(const Request& other)
{
    Component = other.Component;
    httpHeaders = other.httpHeaders;
    returnCode = other.returnCode;
    sFd = other.sFd;
    return *this;
}

bool Request::pathURIChecker(std::string& URI)
{
    for(std::string::iterator i = URI.begin(); i != URI.end(); i++)
    {
        if (!std::isprint(*i) || std::isspace(*i))
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
    // if (Component.method == "POST" && httpHeaders.find("Content-Length") == httpHeaders.end())
    //     return 400;
    if (pathURIChecker(Component.path))
        return 400;
    if (Component.path.size() > 2048)
        return 414;
    if (pathURIChecker(Component.path))
        return 400;
    int cl;
    std::map<std::string, std::string>::iterator it = httpHeaders.find("Content-Length");
        if (it != httpHeaders.end())
            cl = std::atof(httpHeaders.find("Content-Length")->second.c_str());
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
        if(sFd == i->fd && cl > i->maxBodySize)
            return 413;
    return 200;
}

std::string Request::errorPageMessage(void)
{
    if (returnCode == 301)
        return std::string("Moved Permanently");
    if (returnCode == 400)
        return std::string("Bad Request");
    if (returnCode == 405)
        return std::string("Method Not Allowed");
    if (returnCode == 414)
        return std::string("Request URI Too Long");
    if (returnCode == 404)
        return std::string("Not Found");
    if (returnCode == 501)
        return std::string("Not Implemented");
    if (returnCode == 413)
        return std::string("Request Entity Too Large");
    if (returnCode == 200)
        return std::string("OK");
    if (returnCode == 201)
        return std::string("Created");
    return std::string();
}

std::pair<int, std::string> Request::generateCorrespondingErrorPage(void)
{
    if (returnCode != 200 && returnCode != 301)
    {
        std::string fileName = "/home/hamdani/webserv/ErrorPages/" + to_string(returnCode) + ".html";
        std::ifstream content("/home/hamdani/webserv/ErrorPages/error_page.html");
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
                std::string fullPath = i->root + Component.path;
                DIR *dir = opendir(fullPath.c_str());
                if (dir)
                {
                    std::vector<Location>::iterator j;
                    for (j = i->location.begin(); j != i->location.end(); j++)
                    {
                        if (Component.path == j->path)
                        {
                            closedir(dir);
                            return *j;
                        }
                    }
                }
                else if (!dir)
                {
                    closedir(dir);
                    std::string absPath = Component.path.substr(0, Component.path.rfind('/'));
                    std::vector<Location>::iterator j;
                    for (j = i->location.begin(); j != i->location.end(); j++)
                    {
                        if (absPath == j->path)
                            return *j;
                    }
                }
            }
        }
    }
    throw std::invalid_argument("no match");
}

void Request::openErrorPage(Server& srv)
{
    std::map<int, std::string>::iterator i = srv.errPages.find(returnCode);
    if (i != srv.errPages.end())
    {
        std::cout << i->second << std::endl;
        std::ifstream openFile(i->second.c_str());
        if (openFile.is_open())
        {
            struct stat fileStat;
            stat(i->second.c_str(), &fileStat);
            Response& res = static_cast<Response&>(*this);
            res.responseBuffer.append(Component.httpVersion + " ");
            res.responseBuffer.append(to_string(returnCode) + " ");
            res.responseBuffer.append(errorPageMessage() + "\r\n");
            res.responseBuffer.append("Content-Length: ");
            res.responseBuffer.append(to_string(fileStat.st_size));
            res.responseBuffer.append("\r\n");
            res.responseBuffer.append("Content-Type: ");
            res.responseBuffer.append(determineFileExtension(i->second));
            res.responseBuffer.append("\r\n");
            res.file = i->second;
            return;
        }
        else
        {
            generateCorrespondingErrorPage();
            file = "./ErrorPages/" + to_string(returnCode) + ".html";
            srv.errPages.erase(returnCode);
            return;
        }
    }
    else
    {
        generateCorrespondingErrorPage();
        file = "./ErrorPages/" + to_string(returnCode) + ".html";
        srv.errPages.erase(returnCode);
        return;
    }
}

void Request::matchLocation(std::vector<Server>& srv, int whichServer)
{
    Location match;
    try
    {
        match = matchURIWithLocation(srv);
        if (!match.ret.empty())
        {
            Response& res = static_cast<Response&>(*this);
            returnCode = 301;
            res.matchLocation(srv, whichServer);
            return;
        }
        else
        {
            std::vector<std::string>::iterator i = std::find(match.methods.begin(), match.methods.end(), Component.method);
            if (i == match.methods.end())
            {
                returnCode = 405;
                openErrorPage(srv[whichServer]);
                Response& res = static_cast<Response&>(*this);
                res.formTheResponse(srv[whichServer]);
            }
            else
            {
                Response& res = static_cast<Response&>(*this);
                if (Component.method == "GET")
                    res.getMethod(match, srv[whichServer]);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        std::string absPath = Component.path.substr(0, Component.path.rfind('/') + 1);
        if (absPath != "/")
            returnCode = 404;
        std::cout << srv[whichServer].errPages.size() << std::endl;
        openErrorPage(srv[whichServer]);
    }
}

void Request::requestParser(std::string &request, std::vector<Server>& srv)
{
    // std::cout << request << std::endl;
    // exit(0);;
    std::cout << "REQUEST PARSER" << std::endl;
    int whichServer = 0;
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (sFd == i->fd)
            break;
        whichServer++;
    }
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
        strtrim(keyValue.second, " ");
        httpHeaders.insert(keyValue);
        std::getline(stream, line);
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    }
    returnCode = valueChecker(srv);
    std::cout << returnCode << std::endl;
    if (returnCode != 200)
    {
        openErrorPage(srv[whichServer]);
        Response& res = static_cast<Response&>(*this);
        res.formTheResponse(srv[whichServer]);
        std::cout << "here " << std::endl;
        return;
    }
    else if (returnCode == 200)
       matchLocation(srv, whichServer);
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
