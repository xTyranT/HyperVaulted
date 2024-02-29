#include "../includes/Response.hpp"

std::map<std::string, std::string> mimeTypes;

bool validPath(std::string path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void fillMimeTypes(void)
{
    std::ifstream mt("./tools/mime.types");
    std::string line;
    if (!mt.is_open())
        std::cout << strerror(errno) << std::endl;
    while(std::getline(mt, line))
    {
        std::string key = line.substr(0, line.find(':'));
        std::string value = line.substr(line.find(':') + 1);
        mimeTypes[key] = value;
    }
}

Response::Response() : Request()
{
    responseBuffer = std::string();
}

Response::Response(const Response& other) : Request(other)
{
    *this = other;
}

const Response& Response::operator=(const Response& other)
{
    Request::operator=(other);
    responseBuffer = other.responseBuffer;
    return *this;
}

std::string determineFileExtension(std::string fileName)
{
    std::string extension = fileName.substr(fileName.rfind('.') + 1);
    return mimeTypes[extension];
}

std::string directoryHasIndex(std::string path, Location& req)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        for (std::vector<std::string>::iterator i = req.indexes.begin(); i != req.indexes.end(); i++)
        {
            if (entry->d_name[0] == '.')
                continue;
            if (std::string(entry->d_name) == *i)
            {
                closedir(directory);
                return *i;
            }
        }
    }
    closedir(directory);
    return std::string();
}

bool isDirectory(std::string path)
{
    DIR* directory = opendir(path.c_str());
    if (directory)
         return closedir(directory),  true;
    else
        return false;
}

void Response::listDirFiles(std::string path)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    std::vector<std::string> filesList;
    while ((entry = readdir(directory)) != NULL)
        if (entry->d_name[0] != '.')
            filesList.push_back(std::string(entry->d_name));
    closedir(directory);
    responseBuffer.append(Component.httpVersion + " ");
    std::stringstream ret;
    ret << returnCode;
    std::string tmp;
    tmp = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"UTF-8\">\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n  <title>Files</title>\n</head>\n<body>\n\n  <h1>\n";
    for(std::vector<std::string>::iterator i = filesList.begin(); i != filesList.end(); i++)
    {
        tmp.append("<small><a href=\"");
        tmp.append(*i);
        if (isDirectory(path + *i))
        {
            tmp.append("/\"");
            tmp.append(">" + *i + "/" + "</a></small><br>");
        }
        else
            tmp.append("\">" + *i + "</a></small><br>"); // <a href="nightly/">nightly/</a>
    }
    tmp.append("  </h1>\n</body>\n</html>");
    responseBuffer.append(ret.str() + " " + errorPageMessage() + "\r\n" + "Content-Length: ");
    responseBuffer.append(to_string(tmp.size()));
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Type: text/html\r\n");
    responseBuffer.append("\r\n");
    responseBuffer.append(tmp);
}

void Response::formChunkedResponse(Location& req, Server& srv)
{
    // std::cout << "req  " << req.root << std::endl;
    std::string fullPath = req.root + Component.path;
    std::ifstream openedFile(fullPath.c_str());
    if (!openedFile.is_open())
    {
        returnCode = 404;
        openErrorPage(srv);
        formTheResponse(srv);
        return;
    }
    responseBuffer.append(Component.httpVersion + " ");
    responseBuffer.append(to_string(returnCode) + " ");
    responseBuffer.append(errorPageMessage() + "\r\n");
    responseBuffer.append("Content-Type: ");
    responseBuffer.append(determineFileExtension(Component.path.substr(Component.path.rfind('/') + 1)));
    struct stat fileStat;
    stat(fullPath.c_str(), &fileStat);
    responseBuffer.append("\r\nContent Length: " + to_string(fileStat.st_size) + "\r\n\r\n");
    file = fullPath;
}

void Response::getMethod(Location& req, Server& srv)
{
    std::cout << "GET METHOD" << std::endl;
    std::string fullPath = req.root + Component.path.substr(0, Component.path.rfind('/'));
    DIR* directory = NULL;
    directory = opendir(fullPath.c_str());
    if (!directory)
    {
        closedir(directory);
        returnCode = 404;
        openErrorPage(srv);
        formTheResponse(srv);
    }
    else
    {
        if (isDirectory(req.root + Component.path))
        {
            std::cout << "IS DIRECTORY" << std::endl;
            if (Component.path[Component.path.size() - 1] != '/')
            {
                std::cout << "REDIRECT" << std::endl;
                returnCode = 301;
                Component.path.append("/");
                getMethod(req, srv);
                return;
            }
            if (directoryHasIndex(req.root + Component.path, req).empty() && req.autoIndex == true)
            {
                std::cout << "AUTOINDEX" << std::endl;
                returnCode = 200;
                listDirFiles(req.root + Component.path);
                return;
            }
            else if (directoryHasIndex(req.root + Component.path, req).empty() && req.autoIndex == false)
            {
                std::cout << "NO INDEX" << std::endl;
                returnCode = 403;
                openErrorPage(srv);
                formTheResponse(srv);
                return;
            }
            else if (!directoryHasIndex(req.root + Component.path, req).empty() && req.cgi == false)
            {
                std::cout << "INDEX" << std::endl;
                returnCode = 200;
                Component.path.append(directoryHasIndex(req.root + Component.path, req));
                formChunkedResponse(req, srv);
                return;
            }
            else if (!directoryHasIndex(req.root + Component.path, req).empty() && req.cgi == true)
            {
                std::cout << "CGI" << std::endl;
                // run cgi on requested file with GET method
                // return code depend on cgi
                return;
            }
        }
        if (req.cgi == false)
        {
            std::cout << "NOT DIRECTORY" << std::endl;
            struct stat fileStat;
            stat((req.root + Component.path).c_str(), &fileStat);
            if (fileStat.st_mode & S_IFDIR)
            {
                returnCode = 404;
                openErrorPage(srv);
                formTheResponse(srv);
                return;
            }
            else
            {
                std::ifstream file((req.root + Component.path).c_str());
                if (!file.is_open())
                {
                    returnCode = 404;
                    openErrorPage(srv);
                    formTheResponse(srv);
                    return;
                }
                else
                {
                    returnCode = 200;
                    formChunkedResponse(req, srv);
                    return;
                }
            }
            // return the requested file
        }
        else if (req.cgi == true)
        {
            // run cgi on requested file with GET method
            // return code depend on cgi 
            return;
        }
    }
}

void Response::formTheResponse(Server& srv)
{
    responseBuffer.append(Component.httpVersion);
    responseBuffer.append(" ");
    responseBuffer.append(to_string(returnCode) + " ");
    responseBuffer.append(errorPageMessage());
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Length: ");
    std::string filePath = srv.errPages.find(returnCode)->second;
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        std::cout << strerror(errno) << std::endl;
    struct stat fileStat;
    stat(filePath.c_str(), &fileStat);
    responseBuffer.append(to_string(fileStat.st_size));
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Type: ");
    responseBuffer.append(determineFileExtension(Component.path.substr(Component.path.rfind('/') + 1)));
    responseBuffer.append("\r\n");
    responseBuffer.append("\r\n");
}

Response::~Response()
{

}
