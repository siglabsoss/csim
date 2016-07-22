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

void plotter::send(Json::Value jsn)
{

    std::string message = writer.write(jsn);
    //cout << message << endl; //Prints json to be send

    zmq::message_t request(message.size()); //Size of message
    memcpy(request.data(), (message.c_str()), (message.size())); //Copies data into request
    socket->send(request); //Sends off data
} //Sends data off to server
