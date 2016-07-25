#include <core/publisher.hpp>
#include <core/logger.hpp>
#include <errno.h>

Publisher::Publisher(const std::string &topic, size_t msgLen) :
    m_msg(topic.length() + msgLen),
    m_dataOffset(topic.length()),
    m_context(1),
    m_socket(m_context, ZMQ_PUB)
{
    memcpy(m_msg.data(), topic.c_str(), topic.length());
}

bool Publisher::init()
{
    m_socket.bind("tcp://*:5563");

    return true;
}

void Publisher::send(const uint8_t *data)
{
    memcpy((uint8_t *)m_msg.data() + m_dataOffset, data, m_msg.size() - m_dataOffset);
    int retval = m_socket.send(&m_msg, ZMQ_NOBLOCK);
    if (retval < 0) {
        log_err("Publisher::send() - %s", strerror(errno));
    }
}
