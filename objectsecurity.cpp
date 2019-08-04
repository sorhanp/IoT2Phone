//
// Created by sorhanp on 21.7.2019.
//

#include "objectsecurity.h"

ObjectSecurity::ObjectSecurity() {


}

void ObjectSecurity::free_security_object(lwm2m_object_t *objectP) {
    while (objectP->instanceList != nullptr)
    {
        auto * securityInstance = (security_instance_t *)objectP->instanceList;
        objectP->instanceList = objectP->instanceList->next;
        if (nullptr != securityInstance->uri)
        {
            lwm2m_free(securityInstance->uri);
        }
        lwm2m_free(securityInstance);
    }
    lwm2m_free(objectP);
}

char* ObjectSecurity::get_server_uri(lwm2m_object_t *objectP, uint16_t secObjInstID) {
    auto * targetP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, secObjInstID);

    if (nullptr != targetP)
    {
        return lwm2m_strdup(targetP->uri);
    }

    return nullptr;
}

uint8_t ObjectSecurity::prv_get_value(lwm2m_data_t *dataP, security_instance_t *targetP) {
    switch (dataP->id)
    {
        case LWM2M_SECURITY_URI_ID:
            lwm2m_data_encode_string(targetP->uri, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_BOOTSTRAP_ID:
            lwm2m_data_encode_bool(targetP->isBootstrap, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SECURITY_ID:
            lwm2m_data_encode_int(LWM2M_SECURITY_MODE_NONE, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_PUBLIC_KEY_ID:
            // Here we return an opaque of 1 byte containing 0
        {
            uint8_t value = 0;

            lwm2m_data_encode_opaque(&value, 1, dataP);
        }
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
            // Here we return an opaque of 1 byte containing 0
        {
            uint8_t value = 0;

            lwm2m_data_encode_opaque(&value, 1, dataP);
        }
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SECRET_KEY_ID:
            // Here we return an opaque of 1 byte containing 0
        {
            uint8_t value = 0;

            lwm2m_data_encode_opaque(&value, 1, dataP);
        }
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SMS_SECURITY_ID:
            lwm2m_data_encode_int(LWM2M_SECURITY_MODE_NONE, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
            // Here we return an opaque of 6 bytes containing a buggy value
        {
            char * value = "12345";
            lwm2m_data_encode_opaque((uint8_t *)value, 6, dataP);
        }
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
            // Here we return an opaque of 32 bytes containing a buggy value
        {
            char * value = "1234567890abcdefghijklmnopqrstu";
            lwm2m_data_encode_opaque((uint8_t *)value, 32, dataP);
        }
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
            lwm2m_data_encode_int(0, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_SHORT_SERVER_ID:
            lwm2m_data_encode_int(targetP->shortID, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SECURITY_HOLD_OFF_ID:
            lwm2m_data_encode_int(targetP->clientHoldOffTime, dataP);
            return COAP_205_CONTENT;

        default:
            return COAP_404_NOT_FOUND;
    }
}

lwm2m_object_t* ObjectSecurity::get_security_object(){
    lwm2m_object_t* securityObj;

    securityObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (nullptr != securityObj)
    {
        security_instance_t * targetP;

        memset(securityObj, 0, sizeof(lwm2m_object_t));

        securityObj->objID = 0;

        // Manually create an hardcoded instance
        targetP = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
        if (nullptr == targetP)
        {
            lwm2m_free(securityObj);
            return nullptr;
        }

        memset(targetP, 0, sizeof(security_instance_t));
        targetP->instanceId = 0;
        targetP->uri = strdup("coap://localhost:5683");
        targetP->isBootstrap = false;
        targetP->shortID = 123;
        targetP->clientHoldOffTime = 10;

        securityObj->instanceList = LWM2M_LIST_ADD(securityObj->instanceList, targetP);

        securityObj->readFunc = prv_security_read;
    }

    return securityObj;
}

uint8_t ObjectSecurity::prv_security_read(uint16_t instanceId, int *numDataP, lwm2m_data_t **dataArrayP,
                                          lwm2m_object_t *objectP) {
    security_instance_t * targetP;
    uint8_t result;
    int i;

    targetP = (security_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (nullptr == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full instance ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {LWM2M_SECURITY_URI_ID,
                              LWM2M_SECURITY_BOOTSTRAP_ID,
                              LWM2M_SECURITY_SECURITY_ID,
                              LWM2M_SECURITY_PUBLIC_KEY_ID,
                              LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID,
                              LWM2M_SECURITY_SECRET_KEY_ID,
                              LWM2M_SECURITY_SMS_SECURITY_ID,
                              LWM2M_SECURITY_SMS_KEY_PARAM_ID,
                              LWM2M_SECURITY_SMS_SECRET_KEY_ID,
                              LWM2M_SECURITY_SMS_SERVER_NUMBER_ID,
                              LWM2M_SECURITY_SHORT_SERVER_ID,
                              LWM2M_SECURITY_HOLD_OFF_ID};
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == nullptr) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_get_value((*dataArrayP) + i, targetP);
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}


