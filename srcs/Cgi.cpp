#include "../includes/Cgi.hpp"

Cgi::Cgi()
{
    pid = -1;
    executed = false;
    status = -1;
    env = NULL;
    argv = NULL;
    out = NULL;
    in = NULL;
    start = -1;
    end = -1;
    cgiPid = -1;
}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

const Cgi& Cgi::operator=(const Cgi& other)
{
    pid = other.pid;
    executed = other.executed;
    status = other.status;
    env = other.env;
    argv = other.argv;
    out = other.out;
    in = other.in;
    start = other.start;
    end = other.end;
    cgiPid = other.cgiPid;
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

    std::map<std::string, std::string>::iterator it3 = res.httpHeaders.find("Content-Length");
    if (it3 != res.httpHeaders.end())
        env[6] = strdup((tmp[6] + "=" + res.httpHeaders.find("Content-Length")->second).c_str());
    else
        env[6] = strdup((tmp[6] + "=").c_str());
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
    std::string ext;
    try
    {
        ext = fileName.substr(fileName.rfind("."));
    }
    catch(const std::exception& e)
    {
        ext = "";
    }
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
    struct stat fileStat;
    stat((srv.root + "/cgi").c_str(), &fileStat);
    res.responseBuffer = "";
    res.responseBuffer.append(res.Component.httpVersion + " ");
    res.responseBuffer.append(to_string(res.returnCode) + " ");
    res.responseBuffer.append(res.errorPageMessage() + "\r\n");
    std::fstream file;
    if (res.Component.method == "POST")
        file.open(res.postCgiFile.c_str());
    else
        file.open(res.file.c_str());
    if (!file.is_open())
    {
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        return;
    }
    res.file = srv.root + "/cgi";
    return;
}

void Cgi::cgiCaller(Server& srv, Location& req, Response& res, bool& enf)
{
    if (!executed)
    {
        start = clock();
        res.cgiProcessing = true;
        executed = true;
        out = NULL;
        in = NULL;
        env = getEnv(srv, res);
        argv = getArgv(srv, res, req);
        if (res.returnCode != 200)
        {
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            res.cgiProcessing = false;
            res.cgi = false;
            enf = true;
            delete[] env;
            delete[] argv;
            return;
        }
        if (!argv)
        {
            res.cgiProcessing = false;
            req.cgi = false;
            res.cgi = false;
            delete[] env;
            delete[] argv;
            Get(req, srv, res, *this, enf);
            req.cgi = true;
            return;
        }
        pid = fork();
        if (pid == -1)
        {
            res.cgiProcessing = false;
            res.returnCode = 500;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            res.cgi = false;
            enf = true;
            delete[] env;
            delete[] argv;
            return;
        }
        if (pid == 0)
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
    }
    cgiPid = waitpid(pid, &status, WNOHANG);
    end = clock();
    if ((end - start) / CLOCKS_PER_SEC > 3 || cgiPid == -1)
    {
        res.cgiProcessing = false;
        executed = false;
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        enf = true;
        res.cgi = false;
        return;
    }
    if (cgiPid == pid)
    {
        delete[] env;
        delete[] argv;
        executed = false;
        res.returnCode = 200;
        res.cgi = true;
        res.file = srv.root + "/cgi";
        res.cgiProcessing = false;
        enf = true;
        return;
    }
    if (cgiPid == 0)
    {
        res.cgiProcessing = true;
            res.cgi = false;
    }
}