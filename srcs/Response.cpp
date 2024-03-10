#include "../includes/Response.hpp"
#include "../includes/Cgi.hpp"

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
    cgi = false;
    cgiProcessing = false;
    clientPid = -2;
    postCgiFile = std::string();
}

Response::Response(const Response& other) : Request(other)
{
    *this = other;
}

const Response& Response::operator=(const Response& other)
{
    Request::operator=(other);
    responseBuffer = other.responseBuffer;
    postCgiFile = other.postCgiFile;
    cgi = other.cgi;
    clientPid = other.clientPid;
    cgiProcessing = other.cgiProcessing;
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
    std::string tmp = "";
    tmp = getFileTemplate();
    for(std::vector<std::string>::iterator i = filesList.begin(); i != filesList.end(); i++)
    {
        tmp += "<li><span class=\"icon\"></span><a href=\"" + Component.path + *i  + "\">" + *i +"</a></li>";
    }
    tmp.append(" </ul></div></body></html>");
    responseBuffer.append(ret.str() + " " + errorPageMessage() + "\r\n" + "Content-Length: ");
    responseBuffer.append(to_string(tmp.size()));
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Type: text/html\r\n");
    responseBuffer.append("\r\n");
    std::fstream  listdir("/tmp/listdir.html", std::ios::out);
    listdir << tmp;
    file = "/tmp/listdir.html";
}

void Response::formChunkedResponse(Location& req, Server& srv)
{
    responseBuffer = "";
    std::string fullPath = req.root + Component.path;
    std::ifstream openedFile(fullPath.c_str());
    if (!openedFile.is_open())
    {
        returnCode = 404;
        openErrorPage(srv);
        formTheResponse(srv, req);
        return;
    }
    responseBuffer.append(Component.httpVersion + " ");
    responseBuffer.append(to_string(returnCode) + " ");
    responseBuffer.append(errorPageMessage() + "\r\n");
    responseBuffer.append("Content-Type: ");
    std::string extension = determineFileExtension(Component.path.substr(Component.path.rfind('/') + 1));
    if (extension.empty())
        responseBuffer.append("text/html");
    else
        responseBuffer.append(extension);
    struct stat fileStat;
    stat(fullPath.c_str(), &fileStat);
    responseBuffer.append("\r\nContent Length: " + to_string(fileStat.st_size) + "\r\n\r\n");
    file = fullPath;
}

void Response::openErrorPage(Server& srv)
{
    std::map<int, std::string>::iterator i = srv.errPages.find(returnCode);
    if (i != srv.errPages.end())
    {
        std::ifstream openFile(i->second.c_str());
        if (openFile.is_open())
        {
            file = i->second;
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


void Response::formTheResponse(Server& srv, Location& req)
{
    responseBuffer.append(Component.httpVersion);
    responseBuffer.append(" ");
    responseBuffer.append(to_string(returnCode) + " ");
    responseBuffer.append(errorPageMessage());
    if (returnCode == 301)
    {
        responseBuffer.append("\r\n");
        responseBuffer.append("Location: " + req.ret);
    }
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Length: ");
    std::map<int, std::string>::iterator i = srv.errPages.find(returnCode);
    if (i != srv.errPages.end())
        file = i->second;
    else
        file = generateCorrespondingErrorPage().second;
    struct stat fileStat;
    stat(file.c_str(), &fileStat);
    responseBuffer.append(to_string(fileStat.st_size));
    responseBuffer.append("\r\n");
    responseBuffer.append("Content-Type: ");
    if (returnCode != 200)
        responseBuffer.append("text/html");
    else
        responseBuffer.append(determineFileExtension(Component.path.substr(Component.path.rfind('/') + 1)));
    responseBuffer.append("\r\n\r\n");
}

Response::~Response()
{

}
