/*
 * plotter.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: ubuntu
 */

#include <utils/plotter.hpp>
#include <string>
#include <unistd.h>

using namespace std;

plotter *plotter::m_instance = nullptr;

//std::mutex plotter::m_mutex;

void plotter::send(const Json::Value &jsn) const
{
    //std::unique_lock<std::mutex> lock(m_mutex);
    //lock.lock();

    Json::FastWriter writer;
    std::string message = writer.write(jsn);
    //cout << message << endl; //Prints json to be send

    zmq::message_t request(message.size()); //Size of message
    memcpy(request.data(), (message.c_str()), (message.size())); //Copies data into request
    socket->send(request); //Sends off data
    //lock.unlock();
} //Sends data off to server

const plotter &plotter::get()
{
    if (m_instance == nullptr) {
        m_instance = new plotter();
    }
    return *m_instance;
}
