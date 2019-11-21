#include <iostream>
#include <map>
#include <fstream>
#include <cstdint>
#include <cerrno>
#include <cstdlib>
#include <vector>
#include <array>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

using std::cout;
using std::endl;
using std::string;

void logger(std::string message)
{
	std::cout << message << std::endl;
}

void die_with_error(std::string message)
{
	std::cerr << message << std::endl;
	exit(EXIT_FAILURE);
}