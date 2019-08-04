#include <boost/asio.hpp>

#include "liblwm2m.h"
#include "connection.h"
#include "object.h"
#include "objectsecurity.h"
#include "objectserver.h"
#include "objectdevice.h"


void print_state(lwm2m_context_t * lwm2mH)
{
    lwm2m_server_t * targetP;

    fprintf(stderr, "State: ");
    switch(lwm2mH->state)
    {
        case STATE_INITIAL:
            fprintf(stderr, "STATE_INITIAL");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            fprintf(stderr, "STATE_BOOTSTRAP_REQUIRED");
            break;
        case STATE_BOOTSTRAPPING:
            fprintf(stderr, "STATE_BOOTSTRAPPING");
            break;
        case STATE_REGISTER_REQUIRED:
            fprintf(stderr, "STATE_REGISTER_REQUIRED");
            break;
        case STATE_REGISTERING:
            fprintf(stderr, "STATE_REGISTERING");
            break;
        case STATE_READY:
            fprintf(stderr, "STATE_READY");
            break;
        default:
            fprintf(stderr, "Unknown !");
            break;
    }
    fprintf(stderr, "\r\n");

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        fprintf(stderr, "No Bootstrap Server.\r\n");
    }
    else
    {
        fprintf(stderr, "Bootstrap Servers:\r\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stderr, " - Security Object ID %d", targetP->secObjInstID);
            fprintf(stderr, "\tHold Off Time: %lu s", (unsigned long)targetP->lifetime);
            fprintf(stderr, "\tstatus: ");
            switch(targetP->status)
            {
                case STATE_DEREGISTERED:
                    fprintf(stderr, "DEREGISTERED\r\n");
                    break;
                case STATE_BS_HOLD_OFF:
                    fprintf(stderr, "CLIENT HOLD OFF\r\n");
                    break;
                case STATE_BS_INITIATED:
                    fprintf(stderr, "BOOTSTRAP INITIATED\r\n");
                    break;
                case STATE_BS_PENDING:
                    fprintf(stderr, "BOOTSTRAP PENDING\r\n");
                    break;
                case STATE_BS_FINISHED:
                    fprintf(stderr, "BOOTSTRAP FINISHED\r\n");
                    break;
                case STATE_BS_FAILED:
                    fprintf(stderr, "BOOTSTRAP FAILED\r\n");
                    break;
                default:
                    fprintf(stderr, "INVALID (%d)\r\n", (int)targetP->status);
            }
            fprintf(stderr, "\r\n");
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        fprintf(stderr, "No LWM2M Server.\r\n");
    }
    else
    {
        fprintf(stderr, "LWM2M Servers:\r\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stderr, " - Server ID %d", targetP->shortID);
            fprintf(stderr, "\tstatus: ");
            switch(targetP->status)
            {
                case STATE_DEREGISTERED:
                    fprintf(stderr, "DEREGISTERED\r\n");
                    break;
                case STATE_REG_PENDING:
                    fprintf(stderr, "REGISTRATION PENDING\r\n");
                    break;
                case STATE_REGISTERED:
                    fprintf(stderr, "REGISTERED\tlocation: \"%s\"\tLifetime: %lus\r\n", targetP->location, (unsigned long)targetP->lifetime);
                    break;
                case STATE_REG_UPDATE_PENDING:
                    fprintf(stderr, "REGISTRATION UPDATE PENDING\r\n");
                    break;
                case STATE_REG_UPDATE_NEEDED:
                    fprintf(stderr, "REGISTRATION UPDATE REQUIRED\r\n");
                    break;
                case STATE_DEREG_PENDING:
                    fprintf(stderr, "DEREGISTRATION PENDING\r\n");
                    break;
                case STATE_REG_FAILED:
                    fprintf(stderr, "REGISTRATION FAILED\r\n");
                    break;
                default:
                    fprintf(stderr, "INVALID (%d)\r\n", (int)targetP->status);
            }
            fprintf(stderr, "\r\n");
        }
    }
}

connection_t * connection_find(connection_t * connList,
                               struct sockaddr_storage * addr,
                               size_t addrLen)
{
    connection_t * connP;

    connP = connList;
    while (connP != NULL)
    {
        if ((connP->addrLen == addrLen)
            && (memcmp(&(connP->addr), addr, addrLen) == 0))
        {
            return connP;
        }
        connP = connP->next;
    }

    return connP;
}

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{
    client_data_t * app_data;
    connection_t * targetP;

    app_data = (client_data_t *)userData;
    targetP = (connection_t *)sessionH;

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    }
    else
    {
        connection_t * parentP;

        parentP = app_data->connList;
        while (parentP != nullptr && parentP->next != targetP)
        {
            parentP = parentP->next;
        }
        if (parentP != nullptr)
        {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
}

connection_t * connection_new_incoming(connection_t * connList,
                                       int sock,
                                       struct sockaddr * addr,
                                       size_t addrLen)
{
    connection_t * connP;

    connP = (connection_t *)lwm2m_malloc(sizeof(connection_t));
    if (connP != NULL)
    {
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
    }

    return connP;
}

connection_t * connection_create(connection_t * connList,
                                 int sock,
                                 char * host,
                                 char * port,
                                 int addressFamily)
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa;
    socklen_t sl;
    connection_t * connP = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL) return NULL;

    // we test the various addresses
    s = -1;
    for(p = servinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }
    if (s >= 0)
    {
        connP = connection_new_incoming(connList, sock, sa, sl);
        close(s);
    }
    if (NULL != servinfo) {
        freeaddrinfo(servinfo);
    }

    return connP;
}


void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = ObjectSecurity::get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    fprintf(stdout, "Connecting to %s\r\n", uri);

    // parse uri in the form "coaps://[host]:[port]"
    if (0 == strncmp(uri, "coaps://", strlen("coaps://")))
    {
        host = uri+strlen("coaps://");
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://")))
    {
        host = uri+strlen("coap://");
    }
    else
    {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    newConnP = connection_create(dataP->connList, dataP->socket, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        fprintf(stderr, "Connection creation failed.\r\n");
    }
    else {
        dataP->connList = newConnP;
    }

    exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}

int main(int argc, char *argv[]) {
    //io context creation
    boost::asio::io_context ioContext;
    //create connection object
    Connection connection(ioContext, "localhost", "5683", AF_INET6, 56831);
    //connection.send("hello world");

    ////STARTS HERE/////
    lwm2m_context_t * lwm2mH = nullptr;
    char* name = "IoT2Phone";

    Object object;
    const std::vector<lwm2m_object_t*>& lwm2mObjects = object.getMLwm2MObjects();
    lwm2m_object_t* objArray[lwm2mObjects.size()];
    std::copy(lwm2mObjects.begin(), lwm2mObjects.end(), objArray);

    connection.setDataSecObject(objArray[0]);

    lwm2mH = lwm2m_init((void *) &connection.getMData());
    if (nullptr == lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return -1;
    }

    int confResult = lwm2m_configure(lwm2mH, name, nullptr, nullptr, lwm2mObjects.size(), objArray);
    if (confResult != 0)
    {
        fprintf(stderr, "lwm2m_configure() failed: 0x%X\r\n", confResult);
        return -1;
    }

    //START OF LOOP:
    while(true){
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 60;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(connection.getSocket(), &readfds);

    print_state(lwm2mH);

    auto result = lwm2m_step(lwm2mH, &(tv.tv_sec));
    if (result != 0)
    {
        fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
        return -1;
    }

    result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

    if (result < 0)
    {
        if (errno != EINTR)
        {
            fprintf(stderr, "Error in select(): %d %s\r\n", errno, strerror(errno));
        }
    }
    else if (result > 0)
    {
        uint8_t buffer[1024];
        int numBytes;

        /*
         * If an event happens on the socket
         */
        if (FD_ISSET(connection.getSocket(), &readfds))
        {
            struct sockaddr_storage addr;
            socklen_t addrLen;

            addrLen = sizeof(addr);

            /*
             * We retrieve the data received
             */
            numBytes = recvfrom(connection.getSocket(), buffer, 1024, 0, (struct sockaddr *)&addr, &addrLen);

            if (0 > numBytes)
            {
                fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
            }
            else if (0 < numBytes)
            {
                connection_t * connP;

                connP = connection_find(connection.getConnList(), &addr, addrLen);


                if (connP != nullptr)
                {
                    /*
                     * Let liblwm2m respond to the query depending on the context
                     */
                    lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);
                }
                else
                {
                    /*
                     * This packet comes from an unknown peer
                     */
                    fprintf(stderr, "received bytes ignored!\r\n");
                }
            }
        }
    }
}
    //END OF LOOP

    //Close instances
    lwm2m_close(lwm2mH);
//    objectSecurity.free_security_object(objArray[0]);
//    objectServer.free_server_object(objArray[1]);
//    objectDevice.free_object_device(objArray[2]);

    return 0;
}