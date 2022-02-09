#include "irc.hpp"

void sig(int sig)
{
    close(sock);
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "invalid arguments" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);
    irc IRC;

    IRC.launchIrcServer(port, argv[2]);
}
