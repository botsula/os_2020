#include <iostream>
#include <vector>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>

#include <unistd.h>

#include <filesystem>
#include "operations/operations.hpp"
#include <ctime>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

namespace fs = std::filesystem;
char *const envParms[2] = {"STEPLIB=SASC.V6.LINKLIB", NULL};
long int port = 8080;



int main(int argc, char **argv) {

    int status = 0;
    std::vector<std::string> own_progs = {"mycat"};
//    constants
    int end_flag = 0;
    char delimiter = ' ';
//    commands history
    std::vector<std::string> history;
    int saved_stdout;

//    --------------------------- server part -----------------
    int server_flag = 0;
    int server_ok = 0;
    int self_port_flag = 0;

    if (argc >= 2) {
        if (std::string(argv[1]) == "--server") {
            server_flag = 1;

            std::cout << "[myshell] SERVER IS MENTIONED" << std::endl;

            if (argc == 4) {
                if (std::string(argv[2]) == "--port") {
                    std::cout << "[myshell] PORT IS MENTIONED" << std::endl;

                    char *pEnd;
                    port = std::strtol(argv[3], &pEnd, 10);
                    if ((port < 49151) && (port > 1024)) {
                        self_port_flag = 1;
                    } else {
                        std::cout << "[myshell] DEFAULT PORT SETTED" << std::endl;
                    }
                }
            }
        }
    }

    int server_fd, new_socket, valread;
    char buffer[1024] = {0};
    int opt = 1;
    struct sockaddr_in address;


    /* server things */
    if (server_flag == 1) {

        int addrlen = sizeof(address);

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        std::cout << "[myshell] SOCKET PORT: " << port << std::endl;


        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);



        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *) &address,
                 sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }


        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                 (socklen_t *) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "[myshell] SOCKET ADDRESS: " << inet_ntoa(address.sin_addr) << std::endl;
        server_ok = 1;


    }

    saved_stdout = dup(1);
    int server;

//    command line parser
    while (end_flag == 0) {
        std::string previous_command = " - ";
        buffer[1024] = {0};
        std::string s;
        int internal_command = 0;

        int own_prog_flag = 0;    // 0 - very external   /   1 - own
        int comment_flag = 0;
        std::string victim;
        int flag_env = 0; // 0 - current / 1 - copy of
        std::string str;
        std::vector<std::string> tokens;
        std::string token;
        std::string prog_with_path;

        auto path_ptr = getenv("PATH");
        std::string path_var;
        if (path_ptr != nullptr)
            path_var = path_ptr;
        path_var += ":.";
        setenv("PATH", path_var.c_str(), 1);

        std::string path_here;
        char dir[1000];
        getcwd(dir, 1000);
        path_here = dir;
// ----------------------------------------------------------------------------------------------------------------


        if (server_ok == 1) {
            auto shell_pwd = path_here + " $ ";
            send(new_socket, shell_pwd.c_str(), strlen(shell_pwd.c_str()), 0);
            valread = read(new_socket, buffer, 1024);
            token = std::string(buffer);
            server = 1;

        } else {
            server = 0;
            std::cout << path_here << '$' << '\t';
            getline(std::cin, token);
        }

        if (token.find("mexit") != std::string::npos) {  // mexit
            end_flag = 1;

            if (server_ok == 1){
                int s = shutdown(new_socket, 2);
                dup2(saved_stdout, 1);
                std::cout << "[myshell] SHUTDOWN OK: "<< s << std::endl;
                server_ok = 0;
                end_flag = 1;
                exit(EXIT_SUCCESS);
            }
            exit(EXIT_SUCCESS);
            end_flag = 1;
        }


        tokens = parser_of_line(token);

        dup2(new_socket, 1);
        dup2(new_socket, 2);

//        find program name
        if (!tokens.empty()) {
            victim = tokens[0];
            if (victim == "mcd") {                         // mcd

                previous_command = victim;
//                std::cout << "VICTIM " << victim << '\n';
                internal_command = 1;
                if (tokens[1] == ".") {
                    continue;
                } else if (tokens[1] == "..") {
                    chdir("..");
                } else {
                    chdir(tokens[1].c_str());
                    status = errno;
                }
            } else if (victim == "merrno") {
                previous_command = victim;                  // merrno
                std::cout << status << std::endl;
                if (server_ok == 1){
                    std::string socket_status = "" + (char) status;
                    send(new_socket, socket_status.c_str(), strlen(socket_status.c_str()), 0);
                }
                internal_command = 1;
                status = 0;

            } else if (victim == "mpwd") {                 // mpwd
                previous_command = victim;
                internal_command = 1;
                std::cout << path_here << std::endl;
            } else if (victim == "mexport") {              // mexport
                previous_command = victim;
                internal_command = 1;

                if (tokens[1].find("=") != std::string::npos) {
                    std::vector<std::string> vars;
                    boost::split(vars, tokens[1], [](char c) { return c == '='; });
                    if (vars.size() == 2) {
                        status = setenv(vars[0].c_str(), vars[1].c_str(), 1);
                    } else if (vars.size() == 1) {
                        status = setenv(vars[0].c_str(), "", 0);
                    }
                }
            } else if (victim == "myshell") {
                previous_command = victim + " " + tokens[1];
                internal_command = 1;
                myshell_work(tokens[1], flag_env, path_here, own_progs, status, server);
            }

//        (./ - add current path - .) parser
            if (internal_command != 1) {
                std::vector<char> v(victim.begin(), victim.end());
                if (v[0] == '.') {
                    v.erase(v.begin());
                    flag_env = 0;
                    if (v[0] == '/') {
                        flag_env = 1;
                        v.erase(v.begin());
                    }
                }

                std::string s(v.begin(), v.end());

//              check if script
                if (fs::path(s).extension() == ".msh") {

                    previous_command = "myshell script : " + s;
                    internal_command = 1;
                    myshell_work(s, flag_env, path_here, own_progs, status, server);
                } else {
                    // check if progname in own list
                    previous_command = s;
                    prog_with_path = path_finder(own_progs, s, path_here);
                }

            }
            //       create arguments
            std::vector<const char *> arg_for_c;

            executer(prog_with_path, tokens, flag_env, status, server);
            internal_command = 0;
        }

        if (server_ok == 1){
            dup2(saved_stdout, 1);
            dup2(saved_stdout, 2);
            time_t now = time(0);
            char* dt = ctime(&now);
            std::cout << "[myshell] ++" << previous_command << "++ \tfrom " << inet_ntoa(address.sin_addr) << " : " << dt;
            dup2(new_socket, 1);
            dup2(new_socket, 2);
        }
    }
    return 0;

}




