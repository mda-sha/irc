#ifndef CLIENT
#define CLIENT

class Client;
class Channel;
#include "channel.hpp"

std::string makeStringAfterPrefix(std::vector<std::string> cmd);
int checkExistingNicknames(std::string const &nickname, std::vector<Client*>clients);


class Client
{
public:

    int clientSocket;

private:
    bool isAutorized;
    bool password;
    bool away;
    bool oper;
    std::string username;
    std::string realname;
    std::string nick;
    std::string away_msg;
    std::vector<std::string> clientChannels;
    std::vector<std::string> chop;
public:
    Client(int sock) : clientSocket(sock)
    {
        isAutorized = 0;
        away = 0;
        oper = 0;
        nick.clear();
        realname.clear();
        username.clear();
    }

    void addToChannel(Channel *channel)
    {
        clientChannels.push_back(channel->getName());
    }

    bool checkIfOnChannel(std::string name)
    {
        std::vector<std::string>::iterator it = clientChannels.begin();
        std::vector<std::string>::iterator ite = clientChannels.end();
        while (it != ite)
        {
            if (name == *it)
            {
                return true;
            }
            ++it;
        }
        return false;
    }

    bool removeChannel(std::string name)
    {
        std::vector<std::string>::iterator it = clientChannels.begin();
        std::vector<std::string>::iterator ite = clientChannels.end();
        while (it != ite)
        {
            if (name == *it)
            {
                clientChannels.erase(it);
                return true;
            }
        }
        return false;
    }

    void addChop(std::string channelName)
    {
        chop.push_back(channelName);
    }

    std::vector<std::string> getChop()
    {
        return chop;
    }

    void setAwayMsg(std::string const &str) { away_msg = str; }
    std::string getAwayMsg() { return away_msg; }

    std::vector<std::string> getChannels() { return clientChannels; }

    void setAway(bool t_f)
    {
        away = t_f;
    }

    bool getAway() { return away; }

    std::string &getUsername() { return username; }
    std::string &getRealname() { return realname; }

    void makeOper() { oper = true; }

    bool setPassword(std::string g_password, std::string pass, std::vector<Client*> clients, int i)
    {

        if (pass.length() == 0)
        {
            send(clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
            return 0;
        }
        if (isAutorized)
        {
            send(clientSocket, "ERR_ALREADYREGISTRED\n", 20, 0);
            return 1;
        }
        if (pass[0] == ':')
            pass = pass.substr(1, pass.size() - 1);
        if (g_password == pass)
            password = true;
        else
            return 0;
        return 1;
    }
    bool getPass() { return password; }
    void setNick(std::vector<Client*> clients, int i, std::string nickname)
    {
        if ((checkExistingNicknames(nickname, clients)) == -1)
            nick = nickname;
        else
            send(clients[i]->clientSocket, "ERR_NICKCOLLISION\n", 19, 0);
    }

    std::string &getNick()  { return nick; }
    void setName(std::vector<std::string> cmd, int i, std::vector<Client*> clients)
    {
        if (cmd.size() < 5)
        {
            send(clientSocket, "ERR_NEEDMOREPARAMS\n", 20, 0);
            return;
        }
        username = cmd[1];
        if (cmd[4][0] == ':')
            realname = makeStringAfterPrefix(cmd);
        else
            realname = cmd[4];
    }
    void checkIfAutorized()
    {
        if (password && !realname.empty() &&  !nick.empty() && !username.empty())
        {
            isAutorized = true;
            std::string msg;
            msg = ":server 375 " + nick + " :- server Message of the day -\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            msg = ":server 372 " + nick + " : ДАШКА КАКАШКА\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            msg = ":server 376 " + nick + " :End of /MOTD command\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            std::cout << "new registered user " << nick << std::endl;
        }
    }
    bool getIsAutorized()
    {
        return isAutorized;
    }
};

// #include "channel.hpp"
#endif