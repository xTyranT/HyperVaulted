#include "../includes/Response.hpp"
#include "../includes/Cgi.hpp"

bool emptyDirectory(std::string path)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        closedir(directory);
        return false;
    }
    closedir(directory);
    return true;
}

void deleteDirectory(std::string path, Location& req, Server& srv, Response& res)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        if (isDirectory(path + "/" + entry->d_name))
        {
            deleteDirectory(path + "/" + entry->d_name, req, srv, res);
            if (!emptyDirectory(path + "/" + entry->d_name))
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                return;
            }
            rmdir((path + "/" + entry->d_name).c_str());
            res.returnCode = 204;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            closedir(directory);
            return;
        }
        else
        {
            if (remove((path + "/" + entry->d_name).c_str()) == -1)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                return;
            }
            res.returnCode = 204;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            closedir(directory);
            return;
        }
    }
    closedir(directory);
}

void Delete(Location& req, Server& srv, Response& res)
{
    std::string fullPath = req.root + res.Component.path;
    if (!validPath(fullPath))
    {
        res.returnCode = 404;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        return;
    }
    char *path = realpath(fullPath.c_str(), NULL);
    std::string realPath = path;
    char *root = realpath(srv.root.c_str(), NULL);
    std::string rootPath = root;
    if (realPath.compare(0, rootPath.size(), rootPath) != 0)
    {
        res.returnCode = 403;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
        return;
    }
    if (isDirectory(fullPath))
    {
        if (res.Component.path[res.Component.path.size() - 1] != '/')
        {
            res.returnCode = 409;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            return;
        }
        else
        {
            deleteDirectory(fullPath, req, srv, res);
            res.returnCode = 204;
        }
    }
    else
    {
        if (remove(fullPath.c_str()) == -1)
        {
            std::cout << strerror(errno) << std::endl;
            res.returnCode = 403;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            return;
        }
        res.returnCode = 204;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
    }
}