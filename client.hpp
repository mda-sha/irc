#ifndef CLIENT
#define CLIENT

class Client;

#include "channel.hpp"
class Channel;

std::string makeStringAfterPrefix(std::vector<std::string> cmd);
int checkExistingNicknames(std::string const &nickname, std::vector<Client*>clients);
void removeChannel(std::vector<Channel*> &channels, std::string channelName);


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
    std::time_t lastActivity;
    bool pingFlag;
    bool pingSent;


public:
    Client(int sock);
    void setLastActivity();
    int checkLastActivity();
    void pingFlagsToFalse();
    bool getPingSent();
    bool getOper();
    void addToChannel(Channel *channel);
    void deleteFromAllChannels(std::vector<Channel*> &channels);
    bool checkIfOnChannel(std::string name);
    bool removeChannel(std::string name);
    void setAwayMsg(std::string const &str);
    std::string getAwayMsg();
    std::vector<std::string> getChannels();
    void setAway(bool t_f);
    bool getAway();
    std::string &getUsername();
    std::string &getRealname();
    void makeOper();
    bool setPassword(std::string g_password, std::string pass);
    bool getPass() { return password; }
    void setNick(std::vector<Client*> clients, int i, std::string nickname);
    std::string &getNick();
    void setName(std::vector<std::string> cmd);
    void checkIfAutorized();
    bool getIsAutorized();
};

#endif