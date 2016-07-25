#include <zmq.hpp>

class Publisher
{
public:
    Publisher(const std::string &topic, size_t msgLen);
    bool init();
    void send(const uint8_t *data);

private:
    zmq::message_t       m_msg;
    size_t               m_dataOffset;
    zmq::context_t       m_context;
    zmq::socket_t        m_socket;
};
