//
// Created by sorhanp on 14.7.2019.
//

#include "connection.h"

Connection::Connection( boost::asio::io_context &ioContext,
                        const std::string_view &host,
                        const std::string_view &port,
                        const short &protocol,
                        const int &localPort)
                        :
                        m_ioContext(ioContext),
                        m_host(host),
                        m_port(port),
                        m_data(),
                        m_localPort(localPort),
                        m_receiverEndpoint(),
                        m_socket(ioContext)
                        {

    m_data.addressFamily = protocol;

    fprintf(stderr, "Trying to bind LWM2M Client to port %d\r\n", localPort);
    if(protocol == AF_INET){
        boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::udp::v4(), m_localPort);
        m_socket.open(boost::asio::ip::udp::v4());
        m_socket.bind(localEndpoint);
    }
    else if(protocol == AF_INET6){
        boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::udp::v6(), m_localPort);
        m_socket.open(boost::asio::ip::udp::v6());
        m_socket.bind(localEndpoint);
    }

    m_data.socket = m_socket.native_handle();

    if (m_data.socket < 0)
    {
        fprintf(stderr, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
    }
}

Connection::~Connection(){
    m_socket.close();
}

void Connection::send(const std::string_view &message) {
    //test if sending to server works outside of constructor
    boost::asio::ip::udp::resolver resolver(m_ioContext);
    m_receiverEndpoint = *resolver.resolve(m_host, m_port).begin();

    m_socket.send_to(boost::asio::buffer(message, message.size()), m_receiverEndpoint);
}

void Connection::setDataSecObject(lwm2m_object_t *securityObject) {
    m_data.securityObjP = securityObject;
}

const client_data_t &Connection::getMData() const {
    return m_data;
}

const int &Connection::getSocket() const {
    return m_data.socket;
}

connection_t *Connection::getConnList() {
    return m_data.connList;
}

