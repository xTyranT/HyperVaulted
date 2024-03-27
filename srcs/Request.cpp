#include "../includes/Response.hpp"

RequestLine::RequestLine(void)
{
    method = std::string();
    path = std::string();
    httpVersion = std::string();
    realPath = std::string();
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
    realPath = other.realPath;
    qstr = other.qstr;
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
    sindx = -1;
    file = std::string();
    responseBuffer = std::string();
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
    sindx = other.sindx;
    file = other.file;
    responseBuffer = other.responseBuffer;
    matchedLocation = other.matchedLocation;
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

int Request::valueChecker(Server& srv)
{
    std::map<std::string, std::string>::iterator i = httpHeaders.find("Transfer-Encoding");
    if (i != httpHeaders.end() && i->second != "chunked")
        return 501;
    if (i == httpHeaders.end() && httpHeaders.find("Content-Length") == httpHeaders.end() && Component.method == "POST")
        return 411;
    if (Component.httpVersion != "HTTP/1.1")
        return 505;
    if (Component.method != "POST" && Component.method != "DELETE" && Component.method != "GET")
        return 501;
    i =  httpHeaders.find("Content-Type");
    if (i != httpHeaders.end() && i->second.find("multipart/form-data") != std::string::npos)
        return 501;
    if ( Component.method == "POST" && i == httpHeaders.end())
        return 400;
    if (pathURIChecker(Component.path))
        return 400;
    if (Component.path.size() > 2048)
        return 414;
    unsigned long cl;
    std::map<std::string, std::string>::iterator it = httpHeaders.find("Content-Length");
    if (it != httpHeaders.end())
    {
        cl = strtoul (httpHeaders.find("Content-Length")->second.c_str(), NULL, 10);
        if(cl > srv.maxBodySize)
            return 413;
    }
    return 200;
}

std::string Request::errorPageMessage(void)
{
    if (returnCode == 204)
        return std::string("No Content");
    if (returnCode == 301)
        return std::string("Moved Permanently");
    if (returnCode == 400)
        return std::string("Bad Request");
    if (returnCode == 422)
        return std::string("Unprocessable Content");
    if (returnCode == 421)
        return std::string("Misdirected Request");
    if (returnCode == 405)
        return std::string("Method Not Allowed");
    if (returnCode == 414)
        return std::string("Request URI Too Long");
    if (returnCode == 404)
        return std::string("Not Found");
    if (returnCode == 505)
        return std::string("HTTP Version Not Supported");
    if (returnCode == 501)
        return std::string("Not Implemented");
    if (returnCode == 504)
        return std::string("Gateway Timeout");
    if (returnCode == 500)
        return std::string("Internal Server Error");
    if (returnCode == 409)
        return std::string("Conflict");
    if (returnCode == 413)
        return std::string("Request Entity Too Large");
    if (returnCode == 200)
        return std::string("OK");
    if (returnCode == 201)
        return std::string("Created");
    if (returnCode == 411)
        return std::string("Length Required");
    if (returnCode == 408 )
        return std::string("Request Timeout");
    if (returnCode == 403)
        return std::string("Forbidden");
    if (returnCode == 415)
        return std::string("Unsupported Media Type");
    return std::string();
}

std::pair<int, std::string> Request::generateCorrespondingErrorPage(void)
{
    if (returnCode != 200 && returnCode != 301 )
    {
        std::string fileName = "./ErrorPages/" + to_string(returnCode) + ".html";
        std::ifstream content("./ErrorPages/error_page.html");
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
        content.close();
        result.close();
        return std::pair<int, std::string>(returnCode, fileName);
    }
    return std::pair<int, std::string>();
}

Location& Request::matchURIWithLocation(Server& srv, std::string path, bool& enf)
{
    if (!srv.location.empty())
    {
        std::vector<Location>::iterator j;
        for (j = srv.location.begin(); j != srv.location.end(); j++)
            if (path == j->path)
                return *j;
        if (path.at(path.size() - 1) == '/')
            path = path.substr(0, path.rfind('/'));
        else
            path = path.substr(0, path.rfind('/') + 1);
        if (!path.empty())
            return matchURIWithLocation(srv, path, enf);
    }
    throw std::invalid_argument("no match");
}

void Request::checkAllowdUriCharacters(std::string uri)
{
    for(size_t i = 0; i < uri.size(); i++)
    {
        if (!std::isprint(uri[i]))
        {
            returnCode = 400;
            throw std::invalid_argument("invalid uri");
        }
        if (uri[i] == '%')
        {
            i++;
            while(std::isdigit(uri[i]) || (uri[i] >= 'A' && uri[i] <= 'F'))
            {
                i++;
                int j = 0;
                j++;
                if (j > 2)
                    break;
            }
            std::string str = uri.substr(i - 2, 2);
            std::stringstream ss(str);
            int x;
            ss >> std::hex >> x;
            if (x < 33 || x > 126)
            {
                returnCode = 400;
                throw std::invalid_argument("invalid uri");
            }
        }
    }
}

void Request::matchLocation(Server& srv, bool& enf)
{
    Location match;
    try
    {
        match = matchURIWithLocation(srv, Component.path, enf);
        try
        {
            if (Component.path.find('?') == std::string::npos)
                throw std::exception();
            Component.qstr = (Component.path.substr(Component.path.find('?') + 1)).c_str();
            Component.path = Component.path.substr(0, Component.path.find('?'));
        }
        catch(const std::exception& e)
        {
            Component.qstr = "";
        }
        std::string tmp = Component.path.substr(match.path.size());
        if (tmp[0] == '/')
            tmp = tmp.substr(1);
        Component.realPath = match.root + tmp;
        matchedLocation = match;
        if (!match.ret.empty())
        {
            Response& res = static_cast<Response&>(*this);
            returnCode = 301;
            res.formTheResponse(srv, match);
            enf = true;
            return;
        }
        else
        {
            std::vector<std::string>::iterator i = std::find(match.methods.begin(), match.methods.end(), Component.method);
            if (i == match.methods.end())
            { 
                returnCode = 405;
                Response& res = static_cast<Response&>(*this);
                res.openErrorPage(srv);
                res.formTheResponse(srv, match);
                enf = true;
                return;
            }
            if (access(Component.realPath.c_str(), F_OK) != 0)
            {
                returnCode = 404;
                Response& res = static_cast<Response&>(*this);
                res.openErrorPage(srv);
                res.formTheResponse(srv, match);
                enf = true;
                return;
            }
        }
    }
    catch(const std::exception& e)
    {
        returnCode = 404;
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(srv);
        res.formTheResponse(srv, match);
        enf = true;
    }
}

std::vector<std::string> split(std::string str, char delim)
{
    std::vector<std::string> vec;
    std::stringstream stream;
    stream << str;
    std::string line;
    while (std::getline(stream, line, delim))
        vec.push_back(line);
    return vec;
}

Server& Request::matchHostWithServer(std::vector<Server>& srv, std::string request, int srvPort)
{
    std::stringstream stream;
    std::string line;
    stream << request;

    while(std::getline(stream, line))
    {
        if (line.substr(0, 4) == "Host")
            break;
    }
    if (line.empty())
        throw std::invalid_argument("bad request");
    line.erase(remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    std::vector<std::string> splitted = split(line, ':');
    for(std::vector<std::string>::iterator i = splitted.begin(); i != splitted.end(); i++)
        strtrim(*i, " ");
    if (splitted.size() < 2 || splitted.empty())
        throw std::invalid_argument("bad request");
    std::string host;
    int port;
    if (splitted.size() == 3)
    {
        host = splitted[1];
        port = std::atoi(splitted[2].c_str());
    }
    else
    {
        host = splitted[1];
        port = srvPort;
    }
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (std::find(i->srvNames.begin(), i->srvNames.end(), host) != i->srvNames.end() && i->port == port)
        {
            sindx = i - srv.begin();
            return *i;
        }
    }
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (i->host == host && i->port == port)
        {
            sindx = i - srv.begin();
            return *i;
        }
    }
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (i->port == port)
        {
            sindx = i - srv.begin();
            return *i;
        }
    }
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (i->port == srvPort)
        {
            sindx = i - srv.begin();
            return *i;
        }
    }
    throw std::invalid_argument("");
}

void Request::requestParser(std::string &request, std::vector<Server>& srv, bool& enf)
{
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (sFd == i->fd)
        {
            sindx = i - srv.begin();
            break;
        }
    }
    Server server;
    try
    {
        server = matchHostWithServer(srv, request, srv[sindx].port);
    }
    catch(const std::exception& e)
    {
        server = srv[sindx];
        returnCode = 400;
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(server);
        Location loc;
        res.formTheResponse(server, loc);
        enf = true;
        return;
    }
    std::stringstream stream;
    std::string line;
    stream << request;
    std::getline(stream, line);
    line.erase(remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    if (line.empty())
    {
        returnCode = 400;
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(server);
        Location loc;
        res.formTheResponse(server, loc);
        enf = true;
        return;
    }
    std::vector<std::string> splitted = split(line);
    if (splitted.size() != 3)
    {
        returnCode = 400;
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(server);
        Location loc;
        res.formTheResponse(server, loc);
        enf = true;
        return;
    }
    Component.method = *(splitted.begin());
    Component.path = *(splitted.begin() + 1);
    Component.httpVersion = *(splitted.begin() + 2);
    std::getline(stream, line);
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    while (!line.empty())
    {
        std::vector<std::string> splitted = split(line, ':');
        if (splitted.size() < 2)
        {
            returnCode = 400;
            Response& res = static_cast<Response&>(*this);
            res.openErrorPage(server);
            Location loc;
            res.formTheResponse(server, loc);
            enf = true;
            return;
        }
        std::pair<std::string, std::string> keyValue;
        keyValue.first = splitted[0];
        keyValue.second = line.substr(line.find(':') + 1, line.size() - splitted[0].size());
        strtrim(keyValue.second, " ");
        httpHeaders.insert(keyValue);
        std::getline(stream, line);
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    }
    returnCode = valueChecker(server);
    if (returnCode != 200)
    {
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(server);
        Location loc;
        res.formTheResponse(server, loc);
        enf = true;
        return;
    }
    else if (returnCode == 200)
        matchLocation(server, enf);
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

Request::~Request(void)
{

}
