//
// Created by sorhanp on 21.7.2019.
//

/*From Wakaama's example object_security.c:
 *  Resources:
 *          Name            | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Server URI              |  0 |            |  Single   |    Yes    | String  |         |       |
 *  Bootstrap Server        |  1 |            |  Single   |    Yes    | Boolean |         |       |
 *  Security Mode           |  2 |            |  Single   |    Yes    | Integer |   0-3   |       |
 *  Public Key or ID        |  3 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Server Public Key or ID |  4 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Secret Key              |  5 |            |  Single   |    Yes    | Opaque  |         |       |
 *  SMS Security Mode       |  6 |            |  Single   |    Yes    | Integer |  0-255  |       |
 *  SMS Binding Key Param.  |  7 |            |  Single   |    Yes    | Opaque  |   6 B   |       |
 *  SMS Binding Secret Keys |  8 |            |  Single   |    Yes    | Opaque  | 32-48 B |       |
 *  Server SMS Number       |  9 |            |  Single   |    Yes    | Integer |         |       |
 *  Short Server ID         | 10 |            |  Single   |    No     | Integer | 1-65535 |       |
 *  Client Hold Off Time    | 11 |            |  Single   |    Yes    | Integer |         |   s   |
 */

#ifndef IOT2PHONE_OBJECTSECURITY_H
#define IOT2PHONE_OBJECTSECURITY_H

#include <cstring>

#include "liblwm2m.h"

typedef struct _security_instance_
{
    struct _security_instance_ * next;        // matches lwm2m_list_t::next
    uint16_t                     instanceId;  // matches lwm2m_list_t::id
    char *                       uri;
    bool                         isBootstrap;
    uint8_t                      securityMode;
    char *                       publicIdentity;
    uint16_t                     publicIdLen;
    char *                       serverPublicKey;
    uint16_t                     serverPublicKeyLen;
    char *                       secretKey;
    uint16_t                     secretKeyLen;
    uint8_t                      smsSecurityMode;
    char *                       smsParams; // SMS binding key parameters
    uint16_t                     smsParamsLen;
    char *                       smsSecret; // SMS binding secret key
    uint16_t                     smsSecretLen;
    uint16_t                     shortID;
    uint32_t                     clientHoldOffTime;
    uint32_t                     bootstrapServerAccountTimeout;
} security_instance_t;

class ObjectSecurity {
private:

public:
    ObjectSecurity();

    //See if this can be done with destructor
    void free_security_object(lwm2m_object_t * objectP);
    static char* get_server_uri(lwm2m_object_t * objectP,
                          uint16_t secObjInstID);
    static uint8_t prv_get_value(lwm2m_data_t * dataP,
                                 security_instance_t * targetP);
    lwm2m_object_t* get_security_object();
    static uint8_t prv_security_read(uint16_t instanceId,
                              int * numDataP,
                              lwm2m_data_t ** dataArrayP,
                              lwm2m_object_t * objectP);

};


#endif //IOT2PHONE_OBJECTSECURITY_H
