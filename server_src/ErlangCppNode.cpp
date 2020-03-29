#include "ErlangCppNode.hpp"

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>

ErlangCppNode::ErlangCppNode(std::string thisnodename, std::string cookie,
                             short creation)
{
  if (ei_connect_init(&ec, thisnodename.c_str(), cookie.c_str(), creation) < 0)
    throw std::runtime_error("Failed to create a new ei_connect node");
}

ErlangCppNode::ErlangCppNode(
    std::string thishostname, std::string thisalivename,
    std::string thisnodename, Erl_IpAddr thisipaddr, std::string cookie,
    short creation)
{
  if (ei_connect_xinit(&ec, thishostname.c_str(), thisalivename.c_str(),
                       thisnodename.c_str(), thisipaddr, cookie.c_str(),
                       creation) < 0)
    throw std::runtime_error("Failed to create a new ei_connect node");
}

ErlangCppNode::~ErlangCppNode()
{
  unpublish();
  closeServerSocket();
}

void ErlangCppNode::createServerSocket(int maxBacklog)
{
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = 0; // Any
  if (bind(serverSocket, (sockaddr*) &addr, sizeof(addr)) < 0)
    throw std::runtime_error("Failed to bind to the specified port");
  if (listen(serverSocket, maxBacklog) < 0)
    throw std::runtime_error("Failed to start listening");
  socklen_t addr_len = sizeof(addr);
  getsockname(serverSocket, (sockaddr*) &addr, &addr_len);
  if (addr.sin_family != AF_INET)
    throw std::runtime_error("getsockname() returned a different family that AF_INET");
  serverSocketPort = ntohs(addr.sin_port);
}

void ErlangCppNode::closeServerSocket()
{
  if (publishFd != -1)
    unpublish();
  if (serverSocket >= 0)
  {
    close(serverSocket);
    serverSocket = -1;
    serverSocketPort = -1;
  }
}

int ErlangCppNode::accept(ErlConnect& connection)
{
  if (serverSocket == -1)
    throw std::runtime_error("createServerSocket() not called");
  return ei_accept(&ec, serverSocket, &connection);
}

void ErlangCppNode::publish()
{
  if (serverSocketPort == -1)
    throw std::runtime_error("createServerSocket() not called");
  publishFd = ei_publish(&ec, serverSocketPort);
  if (publishFd < 0)
    throw std::runtime_error("Failed to publish the node");
}

void ErlangCppNode::unpublish()
{
  if (publishFd >= 0)
  {
    close(publishFd);
    publishFd = -1;
  }
}

std::string ErlangCppNode::readSystemCookie()
{
  const char* home = getenv("HOME");
  std::ifstream file (std::string(home) + "/.erlang.cookie");
  if (!file)
    return std::string();
  std::string cookie;
  file >> cookie;
  return cookie;
}
