#include "../includes/Cgi.hpp"

int cgifile = 1;

Cgi::Cgi()
{
    pid = -1;
    executed = false;
    status = -1;
    out = NULL;
    in = NULL;
    start = -1;
    end = -1;
    cgiPid = -1;
    cgiExtension = std::string();
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
    out = other.out;
    in = other.in;
    start = other.start;
    end = other.end;
    cgiPid = other.cgiPid;
    cgiExtension = other.cgiExtension;
    return *this;
}

Cgi::~Cgi()
{

}

void Cgi::formCgiResponse(Server& srv, Location& req, Response& res)
{
    struct stat fileStat;
    stat(res.file.c_str(), &fileStat);
    res.responseBuffer = "";
    res.responseBuffer.append(res.Component.httpVersion + " ");
    res.responseBuffer.append(to_string(res.returnCode) + " ");
    res.responseBuffer.append(res.errorPageMessage() + "\r\n");
    if (cgiExtension != ".php")
        res.responseBuffer.append("Content-Type: text/html\r\n\r\n");
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
    file.close();
    return;
}

void Cgi::cgiCaller(Server& srv, Location& req, Response& res, bool& enf)
{
    if (!executed)
    {
        res.cgiProcessing = true;
        executed = true;
        out = NULL;
        in = NULL;
        start = clock();
        pid = fork();
        if (pid == -1)
        {
            res.cgiProcessing = false;
            res.returnCode = 500;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            res.cgi = false;
            enf = true;
            return;
        }
        if (pid == 0)
        {
            char const *env[8];
            char const *argv[3];

            std::vector<std::pair<std::string, std::string> >::iterator it1 = req.cgiPaths.begin();
            if (access(res.Component.realPath.c_str(), F_OK) == -1)
                exit(EXIT_NOT_FOUND);
            std::string ext;
            try
            {
                ext = res.Component.realPath.substr(res.Component.realPath.rfind("."));
                cgiExtension = ext;
            }
            catch(const std::exception& e)
            {
                ext = "";
            }
            for(; it1 != req.cgiPaths.end(); it1++)
            {
                if (ext == it1->first)
                    break;
            }
            if (it1 == req.cgiPaths.end())
                exit(EXIT_NO_CGI);
            argv[0] = it1->second.c_str();
            char absPath[PATH_MAX];
            realpath(res.Component.realPath.c_str(), absPath);
            argv[1] = absPath;
            argv[2] = NULL;

            std::string tmp[7] = { "REQUEST_METHOD", "QUERY_STRING", "REDIRECT_STATUS=200", "SCRIPT_FILENAME", "CONTENT_TYPE", "HTTP_COOKIE", "CONTENT-LENGTH"};
            env[0] = (tmp[0] + "=" + res.Component.method).c_str();
            env[1] = (tmp[1] + "=" + res.Component.qstr).c_str();
            env[2] = tmp[2].c_str();
            if (res.Component.method == "POST")
                env[3] = (tmp[3] + "=" + res.postCgiFile).c_str();
            else
                env[3] = (tmp[3] + "=" + res.Component.realPath).c_str();

            std::map<std::string, std::string>::iterator it = res.httpHeaders.find("Content-Type");
            if (it != res.httpHeaders.end())
                env[4] = (tmp[4] + "=" + it->second).c_str();
            else
                env[4] = (tmp[4] + "=").c_str();

            std::map<std::string, std::string>::iterator it2 = res.httpHeaders.find("Cookie");
            if (it2 != res.httpHeaders.end())
                env[5] = (tmp[5] + "=" + res.httpHeaders.find("Cookie")->second).c_str();
            else
                env[5] = (tmp[5] + "=").c_str();

            std::map<std::string, std::string>::iterator it3 = res.httpHeaders.find("Content-Length");
            if (it3 != res.httpHeaders.end())
                env[6] = (tmp[6] + "=" + res.httpHeaders.find("Content-Length")->second).c_str();
            else
                env[6] = (tmp[6] + "=").c_str();
            env[7] = NULL;
            std::string path;
            if (req.root[req.root.length() - 1] == '/')
                path = req.root + "cgi" + to_string(cgifile);
            else
                path = req.root + "/cgi" + to_string(cgifile);
            out = freopen(path.c_str(), "w", stdout);
            if (!out)
                kill(getpid(), SIGKILL);
            std::string absPath2 = absPath;
            absPath2 = absPath2.substr(0, absPath2.rfind('/'));
            if (res.Component.method == "POST")
            {
                in = freopen(res.postCgiFile.c_str(), "r", stdin);
                if (!in)
                    kill(getpid(), SIGKILL);
            }
            if (chdir(absPath2.c_str()) == -1)
                kill(getpid(), SIGKILL);
            execve(argv[0], (char*const*)argv, (char*const*)env);
            kill(getpid(), SIGKILL);
        }
    }
    cgiPid = waitpid(pid, &status, WNOHANG);
    end = clock();
    if ((end - start) / CLOCKS_PER_SEC > 5)
    {
        res.cgiProcessing = false;
        executed = false;
        res.returnCode = 504;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        if (req.root[req.root.length() - 1] == '/')
            remove((req.root + "cgi" + to_string(cgifile)).c_str());
        else
            remove((req.root + "/cgi" + to_string(cgifile)).c_str());
        res.postCgi = true;
        enf = true;
        res.cgi = false;
        return;
    }
    if (WEXITSTATUS(status) == EXIT_NOT_FOUND)
    {
        res.cgiProcessing = false;
        executed = false;
        res.returnCode = 404;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        if (req.root[req.root.length() - 1] == '/')
            remove((req.root + "cgi" + to_string(cgifile)).c_str());
        else
            remove((req.root + "/cgi" + to_string(cgifile)).c_str());
        res.postCgi = true;
        enf = true;
        res.cgi = false;
        return;
    }
    if (WEXITSTATUS(status) == EXIT_NO_CGI)
    {
        res.cgiProcessing = false;
        res.cgi = false;
        executed = true;
        if (res.Component.method == "GET")
        {
            req.cgi = false;
            Get(req, srv, res, *this, enf);
            req.cgi = true;
        }
        res.postCgi = false;
        enf = true;
        return;
    }
    if (WIFSIGNALED(status))
    {
        res.cgiProcessing = false;
        executed = false;
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        if (req.root[req.root.length() - 1] == '/')
            remove((req.root + "cgi" + to_string(cgifile)).c_str());
        else
            remove((req.root + "/cgi" + to_string(cgifile)).c_str());
        res.postCgi = true;
        enf = true;
        res.cgi = false;
        return;
    }
    if (cgiPid == -1)
    {
        res.cgiProcessing = false;
        executed = false;
        res.returnCode = 500;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        if (req.root[req.root.length() - 1] == '/')
            remove((req.root + "cgi" + to_string(cgifile)).c_str());
        else
            remove((req.root + "/cgi" + to_string(cgifile)).c_str());
        res.postCgi = true;
        enf = true;
        res.cgi = false;
        return;
    } 
    if (cgiPid == pid)
    {
        if (srv.root[srv.root.length() - 1] == '/')
            res.file = srv.root + "cgi" + to_string(cgifile++);
        else
            res.file = srv.root + "/cgi" + to_string(cgifile++);
        executed = false;
        res.returnCode = 200;
        res.postCgi = true;
        res.cgi = true;
        res.cgiProcessing = false;
        enf = true;
        try
        {
            cgiExtension = res.Component.realPath.substr(res.Component.realPath.rfind("."));
        }
        catch(const std::exception& e){}
        return;
    }
    if (cgiPid == 0)
        res.cgiProcessing = true;
}