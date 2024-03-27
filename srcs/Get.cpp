#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

void Get(Location& req, Server& srv, Response& res, Cgi& cgiObj, bool& enf)
{
    if (access(res.Component.realPath.c_str(), F_OK) != 0)
    {
        res.returnCode = 404;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        enf = true;
        return;
    }
    if (access(res.Component.realPath.c_str(), R_OK) != 0)
    {
        res.returnCode = 403;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        enf = true;
        return;
    }
    if (isDirectory(res.Component.realPath))
    {
        if (res.Component.path[res.Component.path.size() - 1] != '/')
        {
            res.returnCode = 301;
            res.Component.path.append("/");
            std::string tmp = req.ret;
            req.ret = res.Component.path;
            res.formTheResponse(srv, req);
            req.ret = tmp;
            enf = true;
            return;
        }
        char path[PATH_MAX];
        char root[PATH_MAX];
        realpath(res.Component.realPath.c_str(), path);
        realpath(req.root.c_str(), root);
        std::string rPath = path;
        std::string rRoot = root;
        if (rPath.find(root) == std::string::npos)
        {
            res.returnCode = 403;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            enf = true;
            return;
        }
        if (directoryHasIndex(res.Component.realPath, req).empty() && req.autoIndex == true)
        {
            if (access((res.Component.realPath).c_str(), R_OK) != 0)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            res.returnCode = 200;
            res.listDirFiles(res.Component.realPath);
            enf = true;
            return;
        }
        else if (directoryHasIndex(res.Component.realPath, req).empty() && req.autoIndex == false)
        {
            res.returnCode = 403;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            enf = true;
            return;
        }
        else if (!directoryHasIndex(res.Component.realPath, req).empty() && req.cgi == false)
        {
            res.Component.realPath.append(directoryHasIndex(res.Component.realPath, req));
            res.returnCode = 200;
            res.formChunkedResponse(req, srv);
            enf = true;
            return;
        }
        else if (!directoryHasIndex(res.Component.realPath, req).empty() && req.cgi == true)
        {
            res.Component.realPath.append(directoryHasIndex(res.Component.realPath, req));
            cgiObj.cgiCaller(srv, req, res, enf);
            if (WEXITSTATUS(cgiObj.status) == EXIT_CGI_SUCCESS)
                cgiObj.formCgiResponse(srv, req, res);
            return;
        }
    }
    if (req.cgi == false)
    {
        struct stat fileStat;
        stat((res.Component.realPath).c_str(), &fileStat);
        if (fileStat.st_mode & S_IFDIR)
        {
            res.returnCode = 404;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            enf = true;
            return;
        }
        else
        {
            if (access((res.Component.realPath).c_str(), F_OK) == -1)
            {
                res.returnCode = 404;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            if (access((res.Component.realPath).c_str(), R_OK) == -1)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            else
            {
                res.returnCode = 200;
                res.formChunkedResponse(req, srv);
                enf = true;
                return;
            }
        }
    }
    else if (req.cgi == true)
    {
        cgiObj.cgiCaller(srv, req, res, enf);
        if (cgiObj.cgiPid == cgiObj.pid)
            cgiObj.formCgiResponse(srv, req, res);
        return;
    }
}
