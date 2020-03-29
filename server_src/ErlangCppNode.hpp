// Original source https://github.com/MCMrARM/ErlangImageNodeExperiment/blob/master/ErlangCNode.cpp
#ifndef ERLANGCPPNODE_HPP
#define ERLANGCPPNODE_HPP
#include <string>
#include <ei.h>

class ErlangCppNode
{
private:
  ei_cnode ec;
  int serverSocket = -1;
  int serverSocketPort = -1;
  int publishFd = -1;

public:
  static std::string readSystemCookie();

  ErlangCppNode(std::string thisnodename,
                std::string cookie = readSystemCookie(),
                short creation = 0);

  ErlangCppNode(std::string thishostname, std::string thisalivename,
                std::string thisnodename, Erl_IpAddr thisipaddr,
                std::string cookie = readSystemCookie(),
                short creation = 0);

  ~ErlangCppNode();

  void createServerSocket(int maxBacklog = 5);

  void closeServerSocket();

  int accept(ErlConnect& connection);

  void publish();

  void unpublish();
};

#endif /*ERLANGCPPNODE_HPP*/
