#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/history.h>

#include <filesystem>
#include "operations/operations.hpp"

namespace fs = std::filesystem;
char *const envParms[2] = {"STEPLIB=SASC.V6.LINKLIB", NULL};


int main(int argc, char **argv) {

    int status = 0;
    std::vector<std::string> own_progs = {"mycat"};
//    constants
    int end_flag = 0;
    char delimiter = ' ';
//    commands history
    std::vector<std::string> history;


//    command line parser
    while (end_flag == 0) {
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
        std::cout << path_here << '$' << '\t';
        getline(std::cin, token);

        if (token.find("mexit") != std::string::npos) {  // mexit
            end_flag = 1;
            exit;
        }


        tokens = parser_of_line(token);
        for (int i = 0; i < tokens.size(); ++i) {
//            std::cout << "Arg " << i << " : " << tokens[i] << '\n';
        }

//        find program name
        if (!tokens.empty()) {
            victim = tokens[0];
            if (victim == "mcd") {                         // mcd
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
            } else if (victim == "merrno") {               // merrno
                std::cout << status;
                internal_command = 1;
                status = 0;

            } else if (victim == "mpwd") {                 // mpwd
                internal_command = 1;

                std::cout << path_here << std::endl;
            } else if (victim == "mexport") {              // mexport
                internal_command = 1;

                if (tokens[1].find("=") != std::string::npos) {
//                    auto point = tokens[1].find("=");
                    std::vector<std::string> vars;
                    boost::split(vars, tokens[1], [](char c) { return c == '='; });
                    if (vars.size() == 2) {
                        status = setenv(vars[0].c_str(), vars[1].c_str(), 1);
                    } else if (vars.size() == 1) {
                        status = setenv(vars[0].c_str(), "", 0);
                    }
                }
            } else if (victim == "myshell") {
                internal_command = 1;
                myshell_work(tokens[1], flag_env, path_here, own_progs, status);
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
                    internal_command = 1;
                    myshell_work(s, flag_env, path_here, own_progs, status);
                } else {
                    // check if progname in own list
                    prog_with_path = path_finder(own_progs, s, path_here);
                }

            }
            //       create arguments
            std::vector<const char *> arg_for_c;

            executer(prog_with_path, tokens, flag_env, status);
            internal_command = 0;
        }

    }
    return 0;

}




