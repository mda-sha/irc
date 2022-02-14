#include "../irc.hpp"

void irc::userhost(std::vector<std::string> cmd, int i)
{
    std::string stringToSend;
    if (cmd.size() == 1)
    {
        stringToSend = ":server 461 " + clients[i]->getNick() + "USERHOST :Not enough parameters\n";
        send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
        return;
    }
    int params = 5;
    std::vector<std::string>::iterator it_cmd = cmd.begin() + 1;
    std::vector<std::string>::iterator ite_cmd = cmd.end();
    stringToSend = ":server 302 " + clients[i]->getNick() + " :";
    while (params-- && it_cmd != ite_cmd)
    {
        std::vector<Client*>::iterator it_cl = clients.begin();
        std::vector<Client*>::iterator ite_cl = clients.end();
        while (it_cl != ite_cl)
        {
            if (*it_cmd == (*it_cl)->getNick())
            {
                stringToSend += *it_cmd;
                stringToSend += "=+@127.0.0.1 ";
            }
            it_cl++;
        }
        it_cmd++;
    }
    stringToSend += "\n";
    send(clients[i]->clientSocket, stringToSend.c_str(), stringToSend.size(), 0);
}