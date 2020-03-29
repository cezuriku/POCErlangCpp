#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <ei.h>

#include "ErlangCppNode.hpp"
#include "ErlangBufferReadHelper.hpp"

class Server
{
private:

  ErlangCppNode node;

  void handleConnection(int fd, ErlConnect connect);

  void handleMessage(int fd, ErlangBufferReadHelper& reader);

  int foo(int x) { return x+1; };
  int bar(int y) { return y*2; };

public:
  Server(std::string nodeName) : node(nodeName) {}

  void run();
};

#endif /*SERVER_HPP*/
