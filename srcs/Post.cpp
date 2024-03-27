
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

int x = 0;
extern std::map<std::string, std::string>  mimeTypes;

int htd( std::string str )
{
    int dec = 0;

    std::istringstream dt( str );

    dt >> std::hex >> dec;

    return dec;
}

std::string itos( std::string path )
{
    std::string name;
    std::stringstream s;
    if ( path[path.size() -1] == '/')
        name = "upload";
    else 
        name = "/upload";
    x++;
    s << name << x  << '.' ; 
    return s.str();
}

std::string gnExtencion( std::string contentType ,  std::string path )
{
    std::string tmp;
    std::map<std::string, std::string>::iterator i;
    std::string ex;
    for ( i = mimeTypes.begin() ; i != mimeTypes.end(); i++ )
    {
        if ( contentType == i->second ){

            ex = i->first;
            break;
        }
    }
    if (ex.empty())
       return "none";
    std::string s =  path + itos(path) + ex;
    if ( access(path.c_str(), F_OK ) == -1)
        return "";
    while ( access(s.c_str(), F_OK ) == 0)
    {
        size_t a = s.rfind('.');
        tmp = s.substr(0 , a) + to_string(x) + s.substr(a);
        s = tmp;
    }
    return s;
}

void    ChunkedPost( Client & Clients , char *buff , int rd , Server & srv, Cgi & cgiObj, bool & enf)
{
    size_t find = 0;
    std::string str;
    if ( !Clients.flag )
    {
        Clients.flag = true;
        std::string fname = gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath);
        if ( fname.empty() )
        {
            Clients.reqRes.returnCode = 404;
            Clients.reqRes.openErrorPage(srv);
            Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
            Clients.enf = true;
            return ;
        }
        if ( !fname.compare("none") )
        {
            Clients.reqRes.returnCode = 415;
            Clients.reqRes.openErrorPage(srv);
            Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
            Clients.enf = true;
            return ;
        }
        Clients.reqRes.postCgiFile = fname;
        Clients.postFile.open(fname.c_str(), std::ios::app );
        if (Clients.postFile.fail())
        {
            Clients.reqRes.returnCode = 500;
            Clients.reqRes.openErrorPage(srv);
            Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
            Clients.enf = true;
            return ;
        }

        find = Clients.request.find("\r\n");
        str  = Clients.request.substr(0 , find);
        Clients.chunksize = htd( str );
        Clients.request = Clients.request.erase(0, find + 2);
        while (  Clients.chunksize != 0  && Clients.chunksize < (size_t)Clients.request.size() )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request = Clients.request.erase( 0 , Clients.chunksize  + 2);
            find = Clients.request.find("\r\n");
            str  = Clients.request.substr(0 , find);
            Clients.chunksize = htd( str );
            Clients.request = Clients.request.erase(0, find + 2);
        }
    }
    else
    {
        Clients.request.append( buff , rd);
        while( Clients.chunksize < (size_t)Clients.request.size() && Clients.request.find("\r\n", Clients.chunksize + 2) != std::string::npos && Clients.chunksize != 0 )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request = Clients.request.erase( 0 , Clients.chunksize + 2 );
            find = Clients.request.find("\r\n");
            str  = Clients.request.substr(0 , find);
            Clients.chunksize = htd( str );
            Clients.request = Clients.request.erase(0, find + 2);
        }
    }
    if ( Clients.chunksize == 0 )
    {
        Clients.enf = true;
        if ( Clients.reqRes.matchedLocation.cgi )
        {
            cgiObj.cgiCaller(srv, Clients.reqRes.matchedLocation, Clients.reqRes, enf);
            if (cgiObj.pid == cgiObj.cgiPid)
            {
                cgiObj.formCgiResponse(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
                Clients.reqRes.postCgi = true;
                Clients.postFile.close();
                return;
            }
        }
        if (WEXITSTATUS(cgiObj.status) == EXIT_NO_CGI || !Clients.reqRes.matchedLocation.cgi)
        {
            Clients.reqRes.returnCode = 201;
            Clients.reqRes.openErrorPage(srv);
            Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
        }
        Clients.postFile.close();
    }
}

size_t _stoi(std::string str)
{
    std::istringstream iss(str);
    size_t nb;
    iss >> nb;
    return nb;
}

void    Post(Client & Clients , char *buff , int rd , Server & srv, Cgi & cgiObj, bool & enf)
{
    std::map<std::string, std::string>::iterator it;
    if ( Clients.reqRes.httpHeaders.find("Transfer-Encoding") != Clients.reqRes.httpHeaders.end() )
            ChunkedPost( Clients , buff , rd , srv, cgiObj, enf);
    else {
        if ( !Clients.flag )
        {
            Clients.flag = true;
            std::string fname =  gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath );
            Clients.reqRes.postCgiFile = fname;
            if ( fname.empty() )
            {
                Clients.reqRes.returnCode = 404;
                Clients.reqRes.openErrorPage(srv);
                Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
                Clients.enf = true;
                return ;
            }
            if ( !fname.compare("none") )
            {
                Clients.reqRes.returnCode = 415;
                Clients.reqRes.openErrorPage(srv);
                Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
                Clients.enf = true;
                return ;
            }
            Clients.postFile.open(fname.c_str(), std::ios::app );
             if (Clients.postFile.fail())
            {
                Clients.reqRes.returnCode = 500;
                Clients.reqRes.openErrorPage(srv);
                Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
                Clients.enf = true;
                return ;
            }
            Clients.contentlength = _stoi( Clients.reqRes.httpHeaders["Content-Length"]);
            if ( Clients.sread >= Clients.contentlength)
            {
                Clients.postFile.write(Clients.request.c_str(), Clients.contentlength);
                Clients.sread = Clients.contentlength;
            }
            else
                Clients.postFile.write(Clients.request.c_str(), Clients.request.size());
        }
        else
        {
            if ( Clients.sread >= Clients.contentlength)
            {
                int x = Clients.sread - Clients.contentlength;
                Clients.postFile.write(buff, rd - x);
                Clients.sread = Clients.contentlength;
            }
            else
                Clients.postFile.write(buff, rd);
        }
        if ( Clients.sread == Clients.contentlength )
        {
            Clients.enf = true;
            if ( Clients.reqRes.matchedLocation.cgi )
            {
                cgiObj.cgiCaller(srv, Clients.reqRes.matchedLocation, Clients.reqRes, enf);
                if (cgiObj.pid == cgiObj.cgiPid)
                {
                    cgiObj.formCgiResponse(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
                    Clients.reqRes.postCgi = true;
                    Clients.postFile.close();
                    return;
                }
            }
            if (WEXITSTATUS(cgiObj.status) == EXIT_NO_CGI || !Clients.reqRes.matchedLocation.cgi)
            {
                Clients.reqRes.returnCode = 201;
                Clients.reqRes.openErrorPage(srv);
                Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
                return;
            }
            Clients.postFile.close();
        }
    }
}
