//
// Created by sorhanp on 20.7.2019.
//

#include "object.h"

Object::Object() {
    m_lwm2mObjects.push_back(m_objectSecurity.get_security_object());
    m_lwm2mObjects.push_back(m_objectServer.get_server_object());
    m_lwm2mObjects.push_back(m_objectDevice.get_object_device());
}

const std::vector<lwm2m_object_t *> &Object::getMLwm2MObjects() const {
    return m_lwm2mObjects;
}
