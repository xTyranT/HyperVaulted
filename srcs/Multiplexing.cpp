
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <fcntl.h>
#include <stdio.h>
#include "../includes/Server.hpp"
#define MAX_EVENTS 10

int    accept_connection( int efd , int fd )
{
    char buffer[1024];
    struct epoll_event event;
    struct sockaddr_in newcon;
    
    char res[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello lfasiii </html>\r\n";

    int len = sizeof( struct sockaddr_in);

    int cfd = accept( fd , reinterpret_cast< struct sockaddr * >(&newcon) , reinterpret_cast<socklen_t*>(&len));


    //fcntl(cfd, F_SETFL, O_NONBLOCK);
    event.data.fd = cfd;
    event.events = EPOLLIN | EPOLLOUT;


    int  rd = read( cfd , buffer , 1024);
    if ( rd == -1 )
        std::cout <<  "read " << std::endl; 
    std::cout << buffer << std::endl;

    if ( epoll_ctl( efd , EPOLL_CTL_ADD , cfd , &event) == -1 )
        return ( perror("ctr"), -1 );

    write(cfd, res, strlen(res));
    return cfd;
}


void    multiplexing( std::vector<Server> & sv )
{
    char buff[1024];
    struct epoll_event ev, events[MAX_EVENTS];
    std::vector<int> sfds;
    std::vector<int> c;

    int efd = epoll_create1(0);
    if ( efd == -1 )
        throw std::invalid_argument(" creat ");
    for (size_t i = 0 ; i < sv.size() ; i++)
    {
        int sfd = socket(AF_INET, SOCK_STREAM, 0);
        if ( sfd == -1 ){
            throw std::invalid_argument(" soket ");
            continue;
        }
        sfds.push_back( sfd );
        int host_adlen = sizeof(sv[i].sockett);
        sv[i].sockett.sin_family = AF_INET;
        sv[i].sockett.sin_port = htons(sv[i].port);
        sv[i].sockett.sin_addr.s_addr = htonl(INADDR_ANY);
        if ( bind(sfd, (struct sockaddr *)&sv[i].sockett , host_adlen) == -1)
            throw std::invalid_argument(" bind ");

        if ( listen(sfd, SOMAXCONN) != 0)
            throw std::invalid_argument(" listen ");
        ev.data.fd = sfd;
        ev.events = EPOLLIN | EPOLLET;
        if ( epoll_ctl(efd, EPOLL_CTL_ADD , sfd , &ev ) == -1)
            throw std::invalid_argument(" ctl ");

    }

    while( 1 ){
        
        int wp = epoll_wait(efd , events , MAX_EVENTS , -1);
        if (wp == -1)
            throw std::invalid_argument(" wait ");
        
        for ( int i = 0  ; i < wp ; i++) 
        {
            int fd = events[i].data.fd;
            std::vector<int>::iterator it = std::find(sfds.begin() , sfds.end(), fd);
            if ( it != sfds.end() )
                c.push_back( accept_connection( efd , fd ));
            else 
            {
                std::cout << "here" <<std::endl;
                int rd = read(fd, buff, 1024);
                if (rd != -1)
                    std::cout << buff << std::endl;
            }
                
        }
    }
}


