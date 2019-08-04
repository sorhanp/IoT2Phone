//
// Created by sorhanp on 14.7.2019.
//

#ifndef PHONE2IOT_CONNECTION_H
#define PHONE2IOT_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "liblwm2m.h"

typedef struct _connection_t
{
    struct _connection_t*   next;
    int                     sock;
    struct sockaddr_in6     addr;
    size_t                  addrLen;
} connection_t;

typedef struct
{
    lwm2m_object_t * securityObjP;
    int socket;
    connection_t * connList;
    int addressFamily;
} client_data_t;

class Connection {
private:
    boost::asio::io_context         &m_ioContext;
    std::string_view                m_host;
    std::string_view                m_port;
    int                             m_localPort;
    boost::asio::ip::udp::socket    m_socket;
    boost::asio::ip::udp::endpoint  m_receiverEndpoint;
    client_data_t                   m_data;
    connection_t                    m_conn;

public:
    Connection(boost::asio::io_context &ioContext,
               const std::string_view &host,
               const std::string_view &port,
               const short &protocol,
               const int &localPort);

    virtual ~Connection();

    void send(const std::string_view &message);

    const client_data_t &getMData() const;
    //TODO: Better implementation of m_data
    void setDataSecObject(lwm2m_object_t *securityObjP);

    const int &getSocket() const;
    connection_t *getConnList();

};

#endif //PHONE2IOT_CONNECTION_H
