#include "../includes/Cgi.hpp"

Cgi::Cgi()
{
    
}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

const Cgi& Cgi::operator=(const Cgi& other)
{
    (void)other;
    return *this;
}

Cgi::~Cgi()
{

}

char** Cgi::getEnv(Server& srv, Response& res)
{
    char **env = new char*[8];
    std::string tmp[7] = { "REQUEST_METHOD", "QUERY_STRING", "REDIRECT_STATUS=200", "SCRIPT_FILENAME", "CONTENT_TYPE", "HTTP_COOKIE", "CONTENT-LENGTH"};
    env[0] = strdup((tmp[0] + "=" + res.Component.method).c_str());
    env[1] = strdup((tmp[1] + "=").c_str());
    env[2] = strdup(tmp[2].c_str());
    if (res.Component.method == "POST")
        env[3] = strdup((tmp[3] + "=" + res.postCgiFile).c_str());
    else
        env[3] = strdup((tmp[3] + "=" + srv.root + res.Component.path).c_str());

    std::map<std::string, std::string>::iterator it = res.httpHeaders.find("Content-Type");
    if (it != res.httpHeaders.end())
        env[4] = strdup((tmp[4] + "=" + it->second).c_str());
    else
        env[4] = strdup((tmp[4] + "=").c_str());

    std::map<std::string, std::string>::iterator it2 = res.httpHeaders.find("Cookie");
    if (it2 != res.httpHeaders.end())
        env[5] = strdup((tmp[5] + "=" + res.httpHeaders.find("Cookie")->second).c_str());
    else
        env[5] = strdup((tmp[5] + "=").c_str());
    env[6] = strdup((tmp[6] + "=" + res.httpHeaders.find("Content-Length")->second).c_str());
    env[7] = NULL;
    return env;
}

char** Cgi::getArgv(Server& srv, Response& res, Location& req)
{
    char **argv = new char*[3];

    std::vector<std::pair<std::string, std::string> >::iterator it = req.cgiPaths.begin();
    char* absPath = NULL;
    absPath = realpath((srv.root + res.Component.path).c_str(), NULL);
    if (absPath == NULL)
    {
        delete[] argv;
        res.returnCode = 404;
        return NULL;
    }
    std::string fileName(absPath);
    std::string ext = fileName.substr(fileName.rfind("."));

    for(; it != req.cgiPaths.end(); it++)
    {
        if (ext == it->first)
            break;
    }
    if (it == req.cgiPaths.end())
    {
        delete[] argv;
        return NULL;
    }
    argv[0] = strdup(it->second.c_str());
    argv[1] = strdup(fileName.c_str());
    argv[2] = NULL;
    return argv;
}

void Cgi::formCgiResponse(Server& srv, Location& req, Response& res)
{
    if (!res.cgi)
        return;
    struct stat fileStat;
    stat((srv.root + "/cgi").c_str(), &fileStat);
    res.responseBuffer.append(res.Component.httpVersion + " ");
    res.responseBuffer.append(to_string(res.returnCode) + " ");
    res.responseBuffer.append(res.errorPageMessage() + "\r\n");
    std::fstream file;
    if (res.Component.method == "POST")
        file.open(res.postCgiFile.c_str());
    else
        file.open(res.file.c_str());    
    std::cout << "file : " << res.file << std::endl;
    std::cout << "post file : " << res.postCgiFile << std::endl;
    if (!file.is_open())
    {
        std::cout << "cgi response file\n";
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        return;
    }
    res.file = srv.root + "/cgi";
    return;
}

void Cgi::cgiCaller(Server& srv, Location& req, Response& res)
{
    FILE* out = NULL;
    FILE* in = NULL;
    char **env = getEnv(srv, res);
    char **argv = getArgv(srv, res, req);
    if (!argv || res.returnCode != 200)
    {
        res.cgi = false;
        delete[] env;
        delete[] argv;
        return;
    }
    res.clientPid = fork();
    if (res.clientPid == 0)
    {
        out = freopen((srv.root + "/cgi").c_str(), "w", stdout);
        if (!out)
            exit(EXIT_FAILURE);
        if (res.Component.method == "POST")
        {
            in = freopen(res.postCgiFile.c_str(), "r", stdin);
            if (!in)
                exit(EXIT_FAILURE);
        }
        execve(argv[0], argv, env);
        exit(EXIT_FAILURE);
    }
    if (res.clientPid == -1)
    {
        res.cgiProcessing = false;
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        delete[] env;
        delete[] argv;
        return;
    }
    if (res.clientPid == 0)
        res.cgiProcessing = true;
    else
    {
        res.cgiProcessing = false;
        // fclose(stdout);
        if (res.Component.method == "POST")
            fclose(stdin);
        delete[] env;
        delete[] argv; 
        res.returnCode = 200;
        res.cgi = true;
        res.file = srv.root + "/cgi";
        std::cout << "cgi is done\n";
        return;
    }
}