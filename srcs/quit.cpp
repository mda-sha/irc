#include "../irc.hpp"

void irc::quit(int i)
{
    clients[i]->deleteFromAllChannels(channels);
    closeAndErase(i);
}
