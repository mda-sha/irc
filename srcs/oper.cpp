#include "../irc.hpp"

void irc::oper(std::vector<std::string> cmd, int i)
{
    if (cmd.size() < 3)
    {
        std::string str = ":server 461 " + clients[i]->getNick() + "OPER :Not enough parameters\n";
        send(clients[i]->clientSocket, str.c_str(), str.size(), 0);
        return;
    }
    int a;
    if ((a = checkExistingNicknames(cmd[1], clients)) != -1 && cmd[2] == oper_password)
    {
        clients[i]->makeOper();
        send(clients[i]->clientSocket, ":server 381 :You are now an IRC operator\n", 42, 0);
    }
}