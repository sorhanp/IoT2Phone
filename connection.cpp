//
// Created by sorhanp on 14.7.2019.
//

#include "connection.h"

Connection::Connection( boost::asio::io_context &ioContext,
                        const short &localPort,
                        const std::string_view &host,
                        const std::string_view &port)
                        :
                        m_ioContext(ioContext),
                        m_host(host),
                        m_port(port),
                        m_localPort(localPort),
                        m_receiverEndpoint(),
                        m_socket(ioContext, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), localPort))
                        {
    boost::asio::ip::udp::resolver resolver(ioContext);
    m_receiverEndpoint = *resolver.resolve(host, port).begin();
}

void Connection::send(const std::string_view &message) {
    m_socket.send_to(boost::asio::buffer(message, message.size()), m_receiverEndpoint);
}

Connection::~Connection(){
    m_socket.close();
}
