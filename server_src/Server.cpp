#include "Server.hpp"

#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "erl_interface.h"
#include "ErlangBufferReadHelper.hpp"

void Server::run() {
    node.createServerSocket();
    node.publish();
    while (true) {
        ErlConnect connection;
        int fd = node.accept(connection);
        if (fd < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::thread thread ([this, fd, connection]() {
            handleConnection(fd, connection);
        });
        thread.detach();
    }
}

void Server::handleConnection(int fd, ErlConnect connect) {
    ei_x_buff buf;
    if (ei_x_new(&buf) < 0)
        throw std::runtime_error("Failed to create connection buffer");
    erlang_msg msg;
    while (true) {
        int r = ei_xreceive_msg(fd, &msg, &buf);
        if (r < 0) {
            std::cerr << connect.nodename << " ei_xreceive_msg failed: "
                      << r << std::endl;
            break;
        }
        if (r == ERL_TICK || msg.msgtype != ERL_REG_SEND)
            continue;
        ErlangBufferReadHelper reader (buf.buff);
        handleMessage(fd, reader);
    }
    ei_x_free(&buf);
    close(fd);
}

void Server::handleMessage(int fd, ErlangBufferReadHelper& reader) {
    reader.decodeVersion();
    int count = reader.decodeTupleHeader();

    std::cout << "Tuple received size " << count << std::endl;
    char atom[MAXATOMLEN];
    erlang_pid pid;
    reader.decodeAtom(atom);
    reader.decodePid(&pid);
    int count2 = reader.decodeTupleHeader();
    std::cout << "Tuple2 received size " << count2 << std::endl;
    reader.decodeAtom(atom);
    std::cout << "Atom received " << atom << std::endl;
    int res = 0;
    if (strcmp(atom, "foo") == 0) {
        int x = (int) reader.decodeLong();
        std::cout << "foo " << x << std::endl;
        res = foo(x);
    } else if (strcmp(atom, "bar") == 0) {
        int x = (int) reader.decodeLong();
        std::cout << "bar " << x << std::endl;
        res = bar(x);
    } else if (strcmp(atom, "gpb") == 0) {
        int x = (int) reader.decodeLong();
        std::cout << " " << x << std::endl;
        res = bar(x);
    }
    char resp[1024];
    int i = 0;
    ei_encode_version(resp, &i);
    ei_encode_tuple_header(resp, &i, 2);
    ei_encode_atom(resp, &i, "cnode");
    ei_encode_long(resp, &i, res);
    ei_send(fd, &pid, resp, i);
}
