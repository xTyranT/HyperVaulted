#pragma once

#include "stringManipulators.hpp"

#define SERVER "server"
#define PORT "listen"
#define HOST "host"
#define ROOT "root"
#define INDEX "index"
#define SRVNAMES "server_name"
#define ERRPAGES "error_page"
#define MAXBDSIZE "max_body_size"
#define LOCATION "location"
#define METHODS "methods"
#define AUTOINDEX "autoindex"
#define UPLOAD "upload"
#define CGI "cgi"
#define UPLOADPATH "upload_path"
#define CGIPATH "cgi_path"
#define RETURN_ "return"

std::string getFileTemplate(void);
