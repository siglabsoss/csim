#pragma once

#include <zmq.hpp>

class Publisher
{
public:
    Publisher(const std::string &topic, size_t msgLen = 0);
    Publisher(const Publisher &other);
    Publisher(Publisher &&other);

    bool init(uint16_t port);
    void send(const uint8_t *data);
    void setMsgLen(size_t msgLen);

private:
    size_t               m_msgLen;
    std::string          m_topic;
    zmq::context_t       m_context;
    zmq::socket_t        m_socket;
};
