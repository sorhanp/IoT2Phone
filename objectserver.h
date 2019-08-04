//
// Created by sorhanp on 28.7.2019.
//

/*From Wakaama's example object_security.c:
 *  Resources:
 *          Name                       | ID | Operations | Instances | Mandatory |  Type    |  Range  | Units |
 *  Short ID                           |  0 |     R      |  Single   |    Yes    | Integer  | 1-65535 |       |
 *  Lifetime                           |  1 |    R/W     |  Single   |    Yes    | Integer  |         |   s   |
 *  Default Min Period                 |  2 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Default Max Period                 |  3 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Disable                            |  4 |     E      |  Single   |    No     |          |         |       |
 *  Disable Timeout                    |  5 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Notification Storing               |  6 |    R/W     |  Single   |    Yes    | Boolean  |         |       |
 *  Binding                            |  7 |    R/W     |  Single   |    Yes    | String   |         |       |
 *  Registration Update                |  8 |     E      |  Single   |    Yes    |          |         |       |
#ifndef LWM2M_VERSION_1_0
 *  Registration Priority Order        | 13 |    R/W     |  Single   |    No     | Unsigned |         |       |
 *  Initial Registration Delay Timer   | 14 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Registration Failure Block         | 15 |    R/W     |  Single   |    No     | Boolean  |         |       |
 *  Bootstrap on Registration Failure  | 16 |    R/W     |  Single   |    No     | Boolean  |         |       |
 *  Communication Retry Count          | 17 |    R/W     |  Single   |    No     | Unsigned |         |       |
 *  Communication Retry Timer          | 18 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Communication Sequence Delay Timer | 19 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Communication Sequence Retry Count | 20 |    R/W     |  Single   |    No     | Unsigned |         |       |
#endif
 *
 */

#ifndef IOT2PHONE_OBJECTSERVER_H
#define IOT2PHONE_OBJECTSERVER_H

#include <cstring>

#include "liblwm2m.h"

typedef struct _server_instance_
{
    struct _server_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    uint16_t    shortServerId;
    uint32_t    lifetime;
    uint32_t    defaultMinPeriod;
    uint32_t    defaultMaxPeriod;
    uint32_t    disableTimeout;
    bool        storing;
    char        binding[4];
#ifndef LWM2M_VERSION_1_0
    int         registrationPriorityOrder; // <0 when it doesn't exist
    int         initialRegistrationDelayTimer; // <0 when it doesn't exist
    int8_t      registrationFailureBlock; // <0 when it doesn't exist, 0 for false, > 0 for true
    int8_t      bootstrapOnRegistrationFailure; // <0 when it doesn't exist, 0 for false, > 0 for true
    int         communicationRetryCount; // <0 when it doesn't exist
    int         communicationRetryTimer; // <0 when it doesn't exist
    int         communicationSequenceDelayTimer; // <0 when it doesn't exist
    int         communicationSequenceRetryCount; // <0 when it doesn't exist
#endif
} server_instance_t;

class ObjectServer {
private:
    lwm2m_object_t*     m_serverObj;
public:
    //Constructor:
    ObjectServer();

    //Methods:
    void free_server_object(lwm2m_object_t* object);
    lwm2m_object_t * get_server_object();
    static uint8_t prv_server_read(uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP);
    static uint8_t prv_get_value(lwm2m_data_t * dataP,
                                 server_instance_t * targetP);
    static uint8_t prv_server_write(uint16_t instanceId,
                                    int numData,
                                    lwm2m_data_t * dataArray,
                                    lwm2m_object_t * objectP);
    static uint8_t prv_set_int_value(lwm2m_data_t * dataArray,
                                     uint32_t * data);
    static uint8_t prv_server_create(uint16_t instanceId,
                                     int numData,
                                     lwm2m_data_t * dataArray,
                                     lwm2m_object_t * objectP);
    static uint8_t prv_server_delete(uint16_t id,
                                     lwm2m_object_t * objectP);
    static uint8_t prv_server_execute(uint16_t instanceId,
                                      uint16_t resourceId,
                                      uint8_t * buffer,
                                      int length,
                                      lwm2m_object_t * objectP);
    static uint8_t prv_server_discover(uint16_t instanceId,
                                       int * numDataP,
                                       lwm2m_data_t ** dataArrayP,
                                       lwm2m_object_t * objectP);
};


#endif //IOT2PHONE_OBJECTSERVER_H
