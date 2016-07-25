#include <zmq.hpp>

class Publisher
{
public:
    Publisher(const std::string &topic, size_t msgLen);
    bool init();
    void send(const uint8_t *data);

private:
    size_t               m_msgLen;
    std::string          m_topic;
    zmq::context_t       m_context;
    zmq::socket_t        m_socket;
};
