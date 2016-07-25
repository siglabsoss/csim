#include <core/publisher.hpp>
#include <core/logger.hpp>
#include <errno.h>

Publisher::Publisher(const std::string &topic, size_t msgLen) :
    m_topic(topic),
    m_msgLen(msgLen),
    m_context(1),
    m_socket(m_context, ZMQ_PUB)
{

}

bool Publisher::init()
{
    m_socket.bind("tcp://*:5563");

    return true;
}

void Publisher::send(const uint8_t *data)
{
    zmq::message_t msg(m_topic.length() + m_msgLen);
    memcpy(msg.data(), m_topic.c_str(), m_topic.length());

    size_t dataOffset = m_topic.length();
    memcpy((uint8_t *)msg.data() + dataOffset, data, msg.size() - dataOffset);
    int retval = m_socket.send(msg, ZMQ_NOBLOCK);
    if (retval < 0) {
        log_err("Publisher::send() - %s", strerror(errno));
    }
}
