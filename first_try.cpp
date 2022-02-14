#include "irc.hpp"

irc IRC;


void sig(int sig)
{
    close(IRC.getSock());
    IRC.eraseAll();
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

    IRC.launchIrcServer(port, argv[2]);
}
