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

std::vector<const char *> create_arguments(std::vector<std::string> tokens);

std::vector<std::string> parser_of_line(std::string token);

std::string path_finder(std::vector<std::string> own_progs, std::string s, std::string path_here);

void executer(std::string prog_with_path, std::vector<std::string> tokens, int flag_env, int status, int server);

void myshell_work(std::string maybe_script, int flag_env, std::string path_here, std::vector<std::string> own_progs,
                  int status, int server);