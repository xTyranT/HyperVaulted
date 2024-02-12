#include "../includes/Response.hpp"

std::map<std::string, std::string> mimeTypes;

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
    responseBuffer = other.responseBuffer;
}

std::string directoryHasIndex(std::string path, Location& req)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        for (std::vector<std::string>::iterator i = req.indexes.begin(); i != req.indexes.end(); i++)
        {
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

void Response::permanentRedirecting(void)
{
    std::cout << "here\n";
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

void Response::getMethod(Location& req)
{
    std::cout << "GET METHOD" << std::endl;
    std::string fullPath = req.root + Component.path.substr(0, Component.path.rfind('/'));
    DIR* directory = NULL;
    directory = opendir(fullPath.c_str());
    if (!directory)
    {
        closedir(directory);
        returnCode = 404;
        generateCorrespondingErrorPage();
        formTheResponse();
    }
    else
    {
        if (isDirectory(req.root + Component.path))
        {
            if (Component.path[Component.path.size() - 1] != '/')
            {
                std::cout << "here\n";
                returnCode = 301;
                Component.path.append("/");
                getMethod(req);
                return;
            }
            if (directoryHasIndex(req.root + Component.path, req).empty() && req.autoIndex == true)
            {
                returnCode = 200;
                listDirFiles(req.root + Component.path);
                return;
            }
            else if (directoryHasIndex(req.root + Component.path, req).empty() && req.autoIndex == false)
            {
                returnCode = 403;
                generateCorrespondingErrorPage();
                formTheResponse();
                return;
            }
            else if (!directoryHasIndex(req.root + Component.path, req).empty() && req.cgi == false)
            {
                returnCode = 200;
                Component.path.append(directoryHasIndex(req.root + Component.path, req));
                // return the requested file
                return;
            }
            else if (!directoryHasIndex(req.root + Component.path, req).empty() && req.cgi == true)
            {
                // run cgi on requested file with GET method
                // return code depend on cgi 
            }
        }
        if (req.cgi == false)
        {
            returnCode = 200;
            // return the requested file
        }
        else if (req.cgi == true)
        {
            // run cgi on requested file with GET method
            // return code depend on cgi 
        }
    }
}

std::string determineFileExtension(std::string fileName)
{
    std::string extension = fileName.substr(fileName.rfind('.') + 1);
    return mimeTypes[extension];
}

std::string Response::appropiateFileLength(void)
{
    if (returnCode != 200 && returnCode != 301)
    {
        std::stringstream tmp;
        tmp << returnCode;
        std::string fileName = "./ErrorPages/" + tmp.str() + ".html";
        std::ifstream file(fileName.c_str());
        if (!file.is_open())
            std::cout << strerror(errno) << std::endl;
        std::stringstream fileString;
        fileString << file.rdbuf();
        std::stringstream size;
        size << fileString.str().size();
        return size.str();
    }
    return std::string();
}

std::string Response::appropiateFileBody(void)
{
    if (returnCode != 200 && returnCode != 301)
    {
        std::stringstream tmp;
        tmp << returnCode;
        std::string fileName = "./ErrorPages/" + tmp.str() + ".html";
        std::ifstream file(fileName.c_str());
        if (!file.is_open())
            std::cout << strerror(errno) << std::endl;
        std::stringstream fileBody;
        fileBody << file.rdbuf();
        return fileBody.str();
    }
    return std::string();
}

void Response::formTheResponse(void)
{
    responseBuffer.append(Component.httpVersion);
    responseBuffer.append(" ");
    std::stringstream ret;
    ret << returnCode;
    responseBuffer.append(ret.str() + " ");
    responseBuffer.append(errorPageMessage());
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Length: ");
    std::string len = appropiateFileLength();
    if (!len.empty())
        responseBuffer.append(len);
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Type: ");
    responseBuffer.append(determineFileExtension(Component.path.substr(Component.path.rfind('/') + 1)));
    responseBuffer.append("\r\n");
    responseBuffer.append("\r\n");
    std::string body = appropiateFileBody();
    if (!body.empty())
        responseBuffer.append(body);
}

void Response::sendResponse(void)
{
    formTheResponse();
    std::cout << responseBuffer.size() << std::endl;
}

Response::~Response()
{

}
