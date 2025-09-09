#include <termios.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include "farsh.cpp"

int main(const int argc, const char* const argv[])
{
	auto f = FarSH();
	f.Run();

	return 0;
}
