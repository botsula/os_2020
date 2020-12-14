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

namespace fs = std::filesystem;
char *const envParms[2] = {"STEPLIB=SASC.V6.LINKLIB", NULL};
char *const envParmsServer[2] = {NULL, NULL};


std::vector<const char *> create_arguments(std::vector<std::string> tokens) {
    std::vector<const char *> arg_for_c;
    for (int i = 0; i < tokens.size(); ++i) {
        if (tokens[i].front() == '#')
            break;

        if (tokens[i].front() == '$') {
            tokens[i].erase(tokens[i].begin());
            auto temp_char = getenv(tokens[i].c_str());
            tokens[i] = temp_char;
        }

        arg_for_c.emplace_back(tokens[i].c_str());
    }
    arg_for_c.push_back(nullptr);

    return arg_for_c;
}

std::vector<std::string> parser_of_line(std::string token) {
    auto found = token.find("\r");
    if (found != std::string::npos){
        token = token.substr(0, found);
    }

    std::vector<std::string> tokens;
    boost::split(tokens, token, [](char c) { return c == ' '; });
//        erase first spaces
    while (tokens.front().find_first_not_of(' ') == std::string::npos) {
        tokens.erase(tokens.begin());
    }
    return tokens;
}

std::string path_finder(std::vector<std::string> own_progs, std::string s, std::string path_here) {
    // check if progname in own list

    std::string prog_with_path;
    int own_prog_flag = 0;
    for (auto prog : own_progs) {
        if (s == prog) {
            own_prog_flag = 1;
            prog_with_path = path_here.append("/") + s;
            break;
        }
    }

    if (own_prog_flag != 1) {
        prog_with_path = "/bin/" + s;
    }

    return prog_with_path;
}

void executer(std::string prog_with_path, std::vector<std::string> tokens, int flag_env, int status, int server) {

    auto arg_for_c = create_arguments(tokens);
    std::vector<std::string> own_progs = {"mycat"};
    char *const *params;

    if (server == 1){
        params = envParmsServer;
    } else {
        params = envParms;
    }

    if (fork() == 0) {
        if (flag_env == 1) {
//            std::cout << "EXTERNAL" << '\n';

            execve(prog_with_path.c_str(), const_cast<char *const *>(arg_for_c.data()),
                   params);
        } else {
//            std::cout << "INTERNAL" << '\n';
            execvp(prog_with_path.c_str(), const_cast<char *const *>(arg_for_c.data()));
        }
        status = errno;
    } else {
        wait(&status);
        status = errno;
    }
}

void myshell_work(std::string maybe_script, int flag_env, std::string path_here, std::vector<std::string> own_progs,
                  int status, int server) {
    if (fs::path(maybe_script).extension() == ".msh") {

        std::ifstream infile(maybe_script);
        std::string line;
        std::istringstream iss(maybe_script);
        while (std::getline(infile, line)) {
            std::vector<std::string> string_arguments = parser_of_line(line);
            std::string script_victim = string_arguments[0];
            auto args_for_c_script = create_arguments(string_arguments);

            auto prog_with_path = path_finder(own_progs, script_victim, path_here);

            executer(prog_with_path, string_arguments, flag_env, status, server);
        }

    } else {
        std::cerr << "The wrong file extension" << std::endl;
    }
}
