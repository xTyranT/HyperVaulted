#include "../includes/webserv.hpp"

std::string getFileTemplate(void)
{
   std::string tmp = "";
   tmp += "<!DOCTYPE html> " + tmp + 
    "<html lang=\"en\"> " +
    "<head> " +
    "  <meta charset=\"UTF-8\"> " +
    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> " +
    "  <title>Cosmic File Listing</title> " +
    "  <style> " +
    "    body { " +
    "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; " +
    "      margin: 0; " +
    "      padding: 0; " +
    "      display: flex; " +
    "      align-items: center; " +
    "      justify-content: center; " +
    "      min-height: 100vh; " +
    "      overflow: hidden; " +
    "      background: url('https://wallpaperset.com/w/full/4/e/d/427170.jpg') no-repeat center center fixed; " +
    "      background-size: cover; " +
    "      color: #fff; " +
    "    } " +
    " " +
    "    .container { " +
    "      text-align: center; " +
    "      position: relative; " +
    "      z-index: 1; " +
    "    } " +
    " " +
    "    h1 { " +
    "      font-size: 3em; " +
    "      margin-bottom: 10px; " +
    "      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5); " +
    "      animation: float 3s ease-in-out infinite; " +
    "    } " +
    " " +
    "    ul { " +
    "      list-style: none; " +
    "      padding: 0; " +
    "      margin: 0; " +
    "      display: flex; " +
    "      flex-direction: column; " +
    "      align-items: center; " +
    "    } " +
    " " +
    "    li { " +
    "      margin: 10px 0; " +
    "      width: 300px; " +
    "      display: flex; " +
    "      align-items: center; " +
    "      justify-content: space-between; " +
    "      padding: 10px; " +
    "      background: rgba(255, 255, 255, 0.1); " +
    "      border-radius: 5px; " +
    "      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); " +
    "      transition: transform 0.3s ease; " +
    "      cursor: pointer; " +
    "    } " +
    " " +
    "    li:hover { " +
    "      transform: scale(1.05); " +
    "    } " +
    " " +
    "    a { " +
    "      text-decoration: none; " +
    "      color: #fff; " +
    "      font-weight: bold; " +
    "    } " +
    " " +
    "    .icon { " +
    "      font-size: 24px; " +
    "      margin-right: 10px; " +
    "    } " +
    " " +
    "    @keyframes float { " +
    "      0%, 100% { " +
    "        transform: translateY(0); " +
    "      } " +
    "      50% { " +
    "        transform: translateY(-20px); " +
    "      } " +
    "    } " +
    "  </style> " +
    "</head> " +
    "<body> " +
    "  <div class=\"container\"> " +
    "    <h1 class=\"glitch\" data-text=\"Cosmic File Listing\">Cosmic File Listing</h1> " +
    "    <ul>";
    return tmp;
}