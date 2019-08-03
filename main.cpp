#include <boost/asio.hpp>

#include "liblwm2m.h"
#include "connection.h"
#include "objectsecurity.h"
#include "objectserver.h"

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
    Connection connection(ioContext, "35.228.251.79", "5683", AF_INET6, 56830);
    connection.send("hello world");

    ////STARTS HERE/////


    lwm2m_context_t * lwm2mH = nullptr;
    lwm2m_object_t * objArray[4];
    char* name = "IoT2Phone";

    ObjectSecurity objectSecurity;
    objArray[0] = objectSecurity.get_security_object();
    if (nullptr == objArray[0])
    {
        fprintf(stderr, "Failed to create security object\r\n");
        return -1;
    }
    connection.setDataSecObject(objArray[0]);

    ObjectServer objectServer;
    objArray[1] = objectServer.get_server_object();
    if (nullptr == objArray[1])
    {
        fprintf(stderr, "Failed to create server object\r\n");
        return -1;
    }

    objArray[2] = get_object_device();
    if (nullptr == objArray[2])
    {
        fprintf(stderr, "Failed to create Device object\r\n");
        return -1;
    }

    //lwm2m_close(lwm2mH);
    objectSecurity.free_security_object(objArray[0]);
    objectServer.free_server_object(objArray[1]);
    free_object_device(objArray[2]);

    return 0;
}