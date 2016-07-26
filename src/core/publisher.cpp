#include <core/publisher.hpp>
#include <core/logger.hpp>
#include <errno.h>


Publisher * Publisher::g_instance = nullptr;

Publisher * Publisher::get()
{
    if (g_instance == nullptr) {
        g_instance = new Publisher();
        g_instance->init();
    }
    return g_instance;
}

Publisher::Publisher() :
    m_context(1),
    m_socket(m_context, ZMQ_PUB)
{

}

bool Publisher::init()
{
    int port = 5555; //XXX retrieve from config
    std::stringstream endpoint;
    endpoint << "tcp://*:" << port;
    m_socket.bind(endpoint.str().c_str());
    log_info("Publisher bound to port %d", port);
    return true;
}

void Publisher::send(const std::string &channel, const uint8_t *data, size_t len)
{
    zmq::message_t msg(channel.length() + len);
    memcpy(msg.data(), channel.c_str(), channel.length());

    size_t dataOffset = channel.length();
    memcpy((uint8_t *)msg.data() + dataOffset, data, msg.size() - dataOffset);
    int retval = m_socket.send(msg, ZMQ_NOBLOCK);
    if (retval < 0) {
        log_err("Publisher::send() - %s", strerror(errno));
    }
}

