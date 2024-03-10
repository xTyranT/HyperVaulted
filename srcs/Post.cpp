
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"
#include <cmath>

int x = 1;
extern std::map<std::string, std::string>  mimeTypes;

int htd( std::string str )
{
    int dec = 0;

    std::istringstream dt( str );

    dt >> std::hex >> dec;

    return dec;
}

std::string itos()
{
    std::string name = "upload";
    std::stringstream s;
    s << name << x  << '.' ;
    x++;
    return s.str();
}

std::string gnExtencion( std::string contentType ,  std::string path )
{
    std::map<std::string, std::string>::iterator i;
    std::string ex;
    for ( i = mimeTypes.begin() ; i != mimeTypes.end(); i++ )
    {
        if ( contentType == i->second){

            ex = i->first;
            break;
        }
    }
    std::string s =  path + itos() + ex;
    return s;
}

void    ChunkedPost( Client & Clients , char *buff , int rd , Server & srv)
{
    size_t find = 0;
    std::string str;
    if ( !Clients.flag )
    {
        Clients.flag = true;
        std::string fname = gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath);
        Clients.reqRes.postCgiFile = fname;
        Clients.postFile.open(fname.c_str(), std::ios::app );

        if (Clients.postFile.fail())
        {
            std::cout<< "open " << fname << std::endl;
            Clients.reqRes.returnCode = 500;
            Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
            Clients.enf = true;
            return ;
        }

        find = Clients.request.find("\r\n");
        str  = Clients.request.substr(0 , find);
        Clients.chunksize = htd( str );
        Clients.request = Clients.request.erase(0, find + 2);
        if ( Clients.chunksize < (size_t)Clients.request.size() )
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
        if ( Clients.chunksize < (size_t)Clients.request.size() && Clients.request.find("\r\n", Clients.chunksize + 2) != std::string::npos )
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
            Cgi postCgi;
            postCgi.cgiCaller(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
            if (Clients.reqRes.returnCode == 200)
                postCgi.formCgiResponse(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
        }
        if (!Clients.reqRes.cgi)
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

void    Post( Client & Clients , char *buff , int rd , Server & srv)
{
    // std::string fullPath = srv.root + Clients.reqRes.Component.path ;
    // if ( !validPath(fullPath))
    // {
    //     Clients.enf = true;
    //     Clients.reqRes.returnCode = 404;
    //     Clients.reqRes.openErrorPage(srv);
    //     Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
    //     return;
    // }
    std::map<std::string, std::string>::iterator it;
    if ( Clients.reqRes.httpHeaders.find("Transfer-Encoding") != Clients.reqRes.httpHeaders.end() )
            ChunkedPost( Clients , buff , rd , srv);
    else {
        if ( !Clients.flag )
        {
            std::cout << Clients.sread << std::endl;
            Clients.flag = true;
            std::string fname =  gnExtencion( Clients.reqRes.httpHeaders["Content-Type"], Clients.reqRes.matchedLocation.uploadPath );
            Clients.reqRes.postCgiFile = fname;
            Clients.postFile.open(fname.c_str(), std::ios::app );
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
                Cgi postCgi;
                postCgi.cgiCaller(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
                if (Clients.reqRes.returnCode == 200)
                    postCgi.formCgiResponse(srv, Clients.reqRes.matchedLocation, Clients.reqRes);
            }
            if (!Clients.reqRes.cgi)
            {
                Clients.reqRes.returnCode = 201;
                Clients.reqRes.openErrorPage(srv);
                Clients.reqRes.formTheResponse(srv, Clients.reqRes.matchedLocation);
            }
            Clients.postFile.close();
        }
    }
}
