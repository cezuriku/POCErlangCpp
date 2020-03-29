#include "Server.hpp"

int main(int /*argc*/, char** /*argv*/)
{
  Server server("cppserver");
  server.run();

  return 0;
}
