#include "irc.hpp"

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
