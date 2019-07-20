//
// Created by sorhanp on 14.7.2019.
//

#ifndef PHONE2IOT_CONNECTION_H
#define PHONE2IOT_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/array.hpp>

class Connection {
private:
    boost::asio::io_context         &m_ioContext;
    boost::asio::ip::udp::socket    m_socket;
    std::string_view                m_host;
    std::string_view                m_port;
    boost::asio::ip::udp::endpoint  m_receiverEndpoint;
    short                           m_localPort;
public:
    Connection(boost::asio::io_context &ioContext,
               const short &localPort,
               const std::string_view &host,
               const std::string_view &port);

    virtual ~Connection();

    void send(const std::string_view &message);
};


#endif //PHONE2IOT_CONNECTION_H
