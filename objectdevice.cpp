//
// Created by sorhanp on 3.8.2019.
//

#include "objectdevice.h"

ObjectDevice::ObjectDevice() {
    m_deviceObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
}

void ObjectDevice::free_object_device(lwm2m_object_t *objectP) {
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    if (NULL != objectP->instanceList)
    {
        lwm2m_free(objectP->instanceList);
        objectP->instanceList = NULL;
    }

    lwm2m_free(objectP);
}

lwm2m_object_t *ObjectDevice::get_object_device() {
    if (nullptr != m_deviceObj)
    {
        memset(m_deviceObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 3 is the standard ID for the mandatory object "Object device".
         */
        m_deviceObj->objID = LWM2M_DEVICE_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        m_deviceObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (nullptr != m_deviceObj->instanceList)
        {
            memset(m_deviceObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(m_deviceObj);
            return nullptr;
        }

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        m_deviceObj->readFunc     = prv_device_read;
        m_deviceObj->executeFunc  = prv_device_execute;
        m_deviceObj->discoverFunc = prv_device_discover;

    }

    return m_deviceObj;
}

uint8_t ObjectDevice::prv_device_read(uint16_t instanceId, int *numDataP, lwm2m_data_t **dataArrayP, lwm2m_object_t *objectP) {
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_MANUFACTURER,
                RES_O_MODEL_NUMBER,
                RES_M_BINDING_MODES
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_set_value((*dataArrayP) + i);
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

uint8_t ObjectDevice::prv_set_value(lwm2m_data_t *dataP) {
    // a simple switch structure is used to respond at the specified resource asked
    switch (dataP->id)
    {
        case RES_O_MANUFACTURER:
            lwm2m_data_encode_string(PRV_MANUFACTURER, dataP);
            return COAP_205_CONTENT;

        case RES_O_MODEL_NUMBER:
            lwm2m_data_encode_string(PRV_MODEL_NUMBER, dataP);
            return COAP_205_CONTENT;

        case RES_M_REBOOT:
            return COAP_405_METHOD_NOT_ALLOWED;

        case RES_M_BINDING_MODES:
            lwm2m_data_encode_string(PRV_BINDING_MODE, dataP);
            return COAP_205_CONTENT;

        default:
            return COAP_404_NOT_FOUND;
    }
}

uint8_t ObjectDevice::prv_device_execute(uint16_t instanceId, uint16_t resourceId, uint8_t *buffer, int length,
                                         lwm2m_object_t *objectP) {
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    if (resourceId == RES_M_REBOOT)
    {
        fprintf(stdout, "\n\t REBOOT\r\n\n");
        return COAP_204_CHANGED;
    }

    return COAP_405_METHOD_NOT_ALLOWED;
}

uint8_t ObjectDevice::prv_device_discover(uint16_t instanceId, int *numDataP, lwm2m_data_t **dataArrayP,
                                          lwm2m_object_t *objectP) {
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_MANUFACTURER,
                RES_O_MODEL_NUMBER,
                RES_M_BINDING_MODES,
                RES_M_REBOOT
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
                case RES_O_MANUFACTURER:
                case RES_O_MODEL_NUMBER:
                case RES_M_BINDING_MODES:
                case RES_M_REBOOT:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

