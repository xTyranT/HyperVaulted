#include "../includes/Response.hpp"
#include "../includes/Cgi.hpp"

bool emptyDirectory(std::string path)
{
    char pathC[PATH_MAX];
    realpath(path.c_str(), pathC);
    DIR* directory = opendir(pathC);
    if (directory == NULL)
        return true;
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            continue;
        closedir(directory);
        return false;
    }
    closedir(directory);
    return true;
}

void deleteDirectory(std::string path, Location& req, Server& srv, Response& res, bool& enf)
{
    DIR* directory = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            continue;
        struct stat s;
        if(stat((path + "/" + entry->d_name).c_str(),&s) == 0)
        {
            if( s.st_mode & S_IFDIR )
            {
                deleteDirectory(path + "/" + entry->d_name, req, srv, res, enf);
                remove((path + "/" + entry->d_name).c_str());
            }
            else if( s.st_mode & S_IFREG )
            {
                if (access((path + "/" + entry->d_name).c_str(), W_OK) == -1)
                    continue;
                remove((path + "/" + entry->d_name).c_str());
            }
            else
            {
                res.returnCode = 422;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                closedir(directory);
                throw std::exception();
            }
        }
        else
        {
            res.returnCode = 422;
            res.openErrorPage(srv);
            res.formTheResponse(srv, req);
            enf = true;
            closedir(directory);
            throw std::exception();
        }
    }
    closedir(directory);
}

void Delete(Location& req, Server& srv, Response& res, bool& enf)
{
    if (access(res.Component.realPath.c_str(), F_OK) == -1)
    {
        res.returnCode = 404;
        res.openErrorPage(srv);
        res.formTheResponse(srv, req);
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
    struct stat s;
    if(stat(res.Component.realPath.c_str(),&s) == 0)
    {
        if( s.st_mode & S_IFDIR )
        {
            if (res.Component.realPath[res.Component.realPath.size() - 1] != '/')
            {
                res.returnCode = 409;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            else
            {
                try
                {
                    deleteDirectory(res.Component.realPath, req, srv, res, enf);
                    if (emptyDirectory(res.Component.realPath) == false)
                    {
                        res.returnCode = 403;
                        res.openErrorPage(srv);
                        res.formTheResponse(srv, req);
                        enf = true;
                        return;
                    }
                    res.returnCode = 204;
                    res.formTheResponse(srv, req);
                    enf = true;
                    return;
                }
                catch(const std::exception& e)
                {
                    return;
                }
            }
        }
        else if( s.st_mode & S_IFREG )
        {
            if (access(res.Component.realPath.c_str(), W_OK) == -1)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            if (remove(res.Component.realPath.c_str()) == -1)
            {
                res.returnCode = 403;
                res.openErrorPage(srv);
                res.formTheResponse(srv, req);
                enf = true;
                return;
            }
            res.returnCode = 204;
            res.formTheResponse(srv, req);
            enf = true;
        }
    }
}