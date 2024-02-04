#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <limits.h>

#define RESPOSE_STATUS 200
#define CONTENT_TYPE "Text"


int main()
{
    std::ostringstream stream;
    int s = socket(AF_INET,SOCK_STREAM,0);
    if(s < 0)
        std::cout << "s " << strerror(errno) << std::endl, exit(1);
    std::cout << s << std::endl;
    struct sockaddr_in fam;
    int len = sizeof(fam);

    fam.sin_addr.s_addr = INADDR_ANY;
    fam.sin_port = htons(8080);
    fam.sin_family = AF_INET;
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&fam,sizeof(len));
    int x = bind(s, (struct sockaddr *)&fam, sizeof(fam));
    if (x < 0)
        std::cout << "x " << strerror(errno) << std::endl, exit(1);
    std::cout << x << std::endl;
    int d = listen(s, 10);
    if (d < 0)
        std::cout << "d " << strerror(errno) << std::endl, exit(1);
    std::cout << d << std::endl;
    while (true)
    {
        int ns = accept(s, (struct sockaddr *)&fam, (socklen_t*)&len);
        if (ns < 0)
            std::cout << "ns " << strerror(errno) << std::endl, exit(1);
        std::cout << ns << std::endl;

        char buffer[1000000];
        int r = read(ns, buffer, 1000000);
        if ( r < 0)
            std::cout << "read\n", exit(1);
        buffer[r] = 0;
        std::cout << buffer << std::endl;
        int o = open("index.html", O_RDWR, 0755);
        if(o < 0)
            std::cout << "open\n", exit(1);
        char str[200000];
        int rea = read(o, str, 20000);
        if(rea < 0)
        {
            std::cout << "read\n", exit (1);
        }
        str[rea] = 0;
        int content = rea;
        std::string tttt = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        stream << content;
        tttt.append(stream.str());
        tttt.append("\n\n");
        tttt.append(str);
        int test = write(ns, tttt.c_str(), tttt.length());
        if (test < 0)
            perror(""), exit(1);
        close(ns);
    }
}