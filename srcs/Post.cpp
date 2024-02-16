
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <cmath>

int htd( std::string str )
{
    int dec = 0;

    std::istringstream dt( str );

    dt >> std::hex >> dec;

    return dec;
}

std::string gnExtencion( std::string contentType )
{
    std::string s = contentType.substr(contentType.find("/") + 1);
    return s;
}

void    ChunkedPost( Client & Clients , char *buff , int rd )
{
    size_t find = 0;
    if ( !Clients.flag )
    {
        Clients.flag = true;
        std::string fname =  "./upload/upload." + gnExtencion( Clients.parsedRequest.httpHeaders["Content-Type"]);
        Clients.postFile.open(fname.c_str(), std::ios::app | std::ios::binary);
        find = Clients.request.find("\r\n");
        Clients.chunksize = htd( Clients.request.substr(0 , find) );
        Clients.request = Clients.request.erase(0, find + 2);
        if ( Clients.chunksize <= (int)Clients.request.size() )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request.erase( 0 , Clients.chunksize  + 2);
            Clients.chunksize = -1;
        }
    }
    else
    {
        Clients.request.append( buff , rd);
        if( Clients.chunksize == -1 ){
            find = Clients.request.find("\r\n");
            Clients.chunksize = htd( Clients.request.substr(0 , find) );
            Clients.request = Clients.request.erase(0, find + 2);
        }
        if ( Clients.chunksize <= (int)Clients.request.size() )
        {
            Clients.postFile.write(Clients.request.c_str(), Clients.chunksize);
            Clients.request.erase( 0 , Clients.chunksize  + 2);
            Clients.chunksize = -1;
        }
        if ( Clients.chunksize == 0 )
        {
            Clients.enf = true;
            Clients.postFile.close();
        }
    }
}

void    Post( Client & Clients , char *buff , int rd )
{
    std::map<std::string, std::string>::iterator it;

    if ( Clients.parsedRequest.httpHeaders.find("Transfer-Encoding") != Clients.parsedRequest.httpHeaders.end() )
    {
        it = Clients.parsedRequest.httpHeaders.find("Transfer-Encoding");
        if ( it->second == " chunked" )
        {
            ChunkedPost( Clients , buff , rd );
        }
        else 
            std::cout << "error " << std::endl;
            
    }
    else if ( !Clients.flag )
    {
        std::string fname =  "./upload/upload." + gnExtencion( Clients.parsedRequest.httpHeaders["Content-Type"]);
        Clients.postFile.open(fname.c_str(), std::ios::app | std::ios::binary);
        Clients.postFile.write(Clients.request.c_str(), Clients.request.size());
        Clients.flag = true;
        Clients.contentlength = atoi(Clients.parsedRequest.httpHeaders["Content-Length"].c_str());
    }
    else
    {
        Clients.postFile.write(buff, rd);
    }
    if ( Clients.sread == Clients.contentlength )
    {
        Clients.enf = true;
        Clients.postFile.close();
    }
}
