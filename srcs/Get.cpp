#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

void Get(Location& req, Server& srv, Response& res, Cgi& cgiObj, bool& enf)
{
    std::string fullPath = req.root + res.Component.path.substr(0, res.Component.path.rfind('/'));
    DIR* directory = NULL;
    directory = opendir(fullPath.c_str());
    if (!directory)
    {
        closedir(directory);
        res.returnCode = 404;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        enf = true;
        return;
    }
    else
    {
        if (isDirectory(req.root + res.Component.path))
        {
            if (res.Component.path[res.Component.path.size() - 1] != '/')
            {
                res.returnCode = 301;
                res.Component.path.append("/");
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            if (directoryHasIndex(req.root + res.Component.path, req).empty() && req.autoIndex == true)
            {
                res.returnCode = 200;
                res.listDirFiles(req.root + res.Component.path);
                enf = true;
                return;
            }
            else if (directoryHasIndex(req.root + res.Component.path, req).empty() && req.autoIndex == false)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            else if (!directoryHasIndex(req.root + res.Component.path, req).empty() && req.cgi == false)
            {
                res.returnCode = 200;
                res.Component.path.append(directoryHasIndex(req.root + res.Component.path, req));
                res.formChunkedResponse(req, srv);
                enf = true;
                return;
            }
            else if (!directoryHasIndex(req.root + res.Component.path, req).empty() && req.cgi == true)
            {
                cgiObj.cgiCaller(srv, req, res, enf);
                if (res.cgiProcessing == false && res.cgi == true)
                    cgiObj.formCgiResponse(srv, req, res);
                if (!cgiObj.argv && res.cgi == false)
                {
                    req.cgi = false;
                    Get(req, srv, res, cgiObj, enf);
                    return;
                }
                return;
            }
        }
        if (req.cgi == false)
        {
            struct stat fileStat;
            stat((req.root + res.Component.path).c_str(), &fileStat);
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
                std::ifstream file((req.root + res.Component.path).c_str());
                if (!file.is_open())
                {
                    res.returnCode = 404;
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
            if (res.cgiProcessing == false && res.cgi == true)
            {
                cgiObj.formCgiResponse(srv, req, res);
            }
            if (!cgiObj.argv && req.cgi == false)
            {
                req.cgi = false;
                Get(req, srv, res, cgiObj, enf);
                return;
            }
            return;
        }
    }
}