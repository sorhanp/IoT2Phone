#include <iostream>
#include <boost/asio.hpp>

#include "liblwm2m.h"
#include "connection.h"

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{

}

void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    return userData;
}

int main(int argc, char *argv[]) {
    //io context creation
    boost::asio::io_context ioContext;
    //create connection object
    Connection connection(ioContext, 56830, "35.228.251.79", "5683");
    connection.send("hello world");


    return 0;
}