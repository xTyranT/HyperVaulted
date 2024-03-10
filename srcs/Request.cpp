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
    sindx = other.sindx;
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
    // Location without POST return 401
    std::map<std::string, std::string>::iterator i = httpHeaders.find("Transfer-Encoding");
    if (i != httpHeaders.end() && i->second != "chunked")
        return 501;
    if (i == httpHeaders.end() && httpHeaders.find("Content-Length") == httpHeaders.end() && Component.method == "POST")
        return 411;
    if (Component.method != "POST" && Component.method != "DELETE" && Component.method != "GET")
        return 501;
    if ( httpHeaders.find("Content-Type") == httpHeaders.end() && Component.method == "POST")
        return 400;
    i =  httpHeaders.find("Content-Type");
    if ( i != httpHeaders.end() && i->second.find("multipart/form-data") != std::string::npos)
        return 501;
    if (pathURIChecker(Component.path))
        return 400;
    if (Component.path.size() > 2048)
        return 414;
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
    if (returnCode == 204)
        return std::string("No Content");
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
    return std::string();
}

std::pair<int, std::string> Request::generateCorrespondingErrorPage(void)
{
    if (returnCode != 200 && returnCode != 301 )
    {
        std::string fileName = "./ErrorPages/" + to_string(returnCode) + ".html";
        std::ifstream content("./ErrorPages//error_page.html");
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

Location& Request::matchURIWithLocation(std::vector<Server>& srv, std::string path)
{
    std::vector<Server>::iterator i;
    for (i = srv.begin(); i != srv.end(); i++)
    {
        if (sFd == i->fd)
        {
            if (!i->location.empty())
            {
                std::string fullPath = i->root + path;
                DIR *dir = opendir(fullPath.c_str());
                if (dir)
                {
                    std::vector<Location>::iterator j;
                    for (j = i->location.begin(); j != i->location.end(); j++)
                    {
                        if (path == j->path)
                        {
                            closedir(dir);
                            return *j;
                        }
                    }
                    if (path.at(path.size() - 1) == '/')
                        path = path.substr(0, path.rfind('/'));
                    else
                        path = path.substr(0, path.rfind('/') + 1);
                    if (!path.empty())
                        return matchURIWithLocation(srv, path);
                }
                else if (!dir)
                {
                    closedir(dir);
                    std::string absPath = Component.path.substr(0, Component.path.rfind('/') + 1);
                    std::vector<Location>::iterator j;
                    for (j = i->location.begin(); j != i->location.end(); j++)
                    {
                        if (absPath == j->path)
                            return *j;
                    }
                    if (path.at(path.size() - 1) == '/')
                        path = path.substr(0, path.rfind('/'));
                    else
                        path = path.substr(0, path.rfind('/') + 1);
                    if (!path.empty())
                        return matchURIWithLocation(srv, path);
                }
            }
        }
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

void Request::matchLocation(std::vector<Server>& srv, int whichServer)
{
    Location match;
    try
    {
        // checkAllowdUriCharacters(Component.path);
        match = matchURIWithLocation(srv, Component.path);
        matchedLocation = match;
        if (!match.ret.empty())
        {
            Response& res = static_cast<Response&>(*this);
            returnCode = 301;
            res.openErrorPage(srv[whichServer]);
            res.formTheResponse(srv[whichServer], match);
            return;
        }
        else
        {
            std::vector<std::string>::iterator i = std::find(match.methods.begin(), match.methods.end(), Component.method);
            if (i == match.methods.end())
            { 
                returnCode = 405;
                Response& res = static_cast<Response&>(*this);
                res.openErrorPage(srv[whichServer]);
                res.formTheResponse(srv[whichServer], match);
            }
            else
            {
                Response& res = static_cast<Response&>(*this);
                if (Component.method == "GET")
                    res.getMethod(match, srv[whichServer]);
                else if (Component.method == "DELETE")
                    res.deleteMethod(match, srv[whichServer]);
                else if (Component.method == "POST" && match.upload == false)
                {
                    returnCode = 409;
                    res.openErrorPage(srv[whichServer]);
                    res.formTheResponse(srv[whichServer], match);
                    return;
                }
                return;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::string absPath = Component.path.substr(0, Component.path.rfind('/') + 1);
        if (absPath != "/")
            returnCode = 404;
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(srv[whichServer]);
        res.formTheResponse(srv[whichServer], match);
    }
}

void Request::requestParser(std::string &request, std::vector<Server>& srv)
{
    int whichServer = 0;
    for(std::vector<Server>::iterator i = srv.begin(); i != srv.end(); i++)
    {
        if (sFd == i->fd)
            break;
        whichServer++;
    }
    sindx = whichServer;
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
    if (returnCode != 200)
    {
        Response& res = static_cast<Response&>(*this);
        res.openErrorPage(srv[whichServer]);
        Location loc;
        res.formTheResponse(srv[whichServer], loc);
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

Request::~Request(void)
{

}
