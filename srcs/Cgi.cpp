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
    char **env = new char*[7];
    std::string tmp[6] = { "REQUEST_METHOD", "QUERY_STRING", "REDIRECT_STATUS=200", "SCRIPT_FILENAME", "CONTENT_TYPE", "HTTP_COOKIE"};
    env[0] = strdup((tmp[0] + "=" + res.Component.method).c_str());
    env[1] = strdup((tmp[1] + "=").c_str());
    env[2] = strdup(tmp[2].c_str());
    env[3] = strdup((tmp[3] + "=" + srv.root + res.Component.path + "index.php").c_str());

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

    env[6] = NULL;
    return env;
}

char** Cgi::getArgv(Server& srv, Response& res, Location& req)
{
    char **argv = new char*[3];

    std::vector<std::pair<std::string, std::string> >::iterator it3 = req.cgiPaths.begin();
    std::string absPath = realpath((srv.root + res.Component.path).c_str(), NULL);

    std::string fileName = absPath + res.Component.path + "index.php";

    std::string ext = fileName.substr(fileName.rfind("."));

    for(; it3 != req.cgiPaths.end(); it3++)
    {
        if (ext == it3->first)
            break;
    }
    argv[0] = strdup(it3->second.c_str());
    argv[1] = strdup(fileName.c_str());
    argv[2] = NULL;
    return argv;
}

void Cgi::cgiCaller(Server& srv, Location& req, Response& res)
{
    (void)srv;
    (void)req;

    pid_t pid;
    int status;
    char **env = getEnv(srv, res);
    char **argv = getArgv(srv, res, req);
    
    pid = fork();
    if (pid < 0)
    {
        std::cerr << "fork failed" << std::endl;
    }
    else if (pid == 0)
    {
        FILE* fd = freopen("tmp", "w", stdout);
        (void)fd;
        execve(argv[0], argv, env);
    }
    waitpid(pid, &status, 0);
    fclose(stdout);
}