//
// Created by sorhanp on 20.7.2019.
//

#ifndef IOT2PHONE_OBJECT_H
#define IOT2PHONE_OBJECT_H

#include <vector>
#include "liblwm2m.h"

#include "objectsecurity.h"
#include "objectserver.h"
#include "objectdevice.h"

class Object {
private:
    std::vector<lwm2m_object_t*> m_lwm2mObjects;

    ObjectSecurity m_objectSecurity;
    ObjectServer m_objectServer;
    ObjectDevice m_objectDevice;
public:
    Object();

    const std::vector<lwm2m_object_t *> &getMLwm2MObjects() const;
};


#endif //IOT2PHONE_OBJECT_H
