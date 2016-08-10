#pragma once

#include <3rd/zmq.hpp>

class Publisher
{
public:
    void send(const std::string &channel, const uint8_t *data, size_t len);
    static Publisher *get();

private:
    Publisher();
    bool init();

    zmq::context_t       m_context;
    zmq::socket_t        m_socket;

    static Publisher *   g_instance;
};
