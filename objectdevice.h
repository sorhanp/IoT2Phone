//
// Created by sorhanp on 3.8.2019.
//

/*From Wakaama's example object_device.c
 * This object is single instance only, and is mandatory to all LWM2M device as it describe the object such as its
 * manufacturer, model, etc...
 */

#ifndef IOT2PHONE_OBJECTDEVICE_H
#define IOT2PHONE_OBJECTDEVICE_H

#include "liblwm2m.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#define PRV_MANUFACTURER      "sorhanp"
#define PRV_MODEL_NUMBER      "IoT2Phone 0.1"
#define PRV_SERIAL_NUMBER     "345000123"
#define PRV_FIRMWARE_VERSION  "1.0"
#define PRV_POWER_SOURCE_1    1
#define PRV_POWER_SOURCE_2    5
#define PRV_POWER_VOLTAGE_1   3800
#define PRV_POWER_VOLTAGE_2   5000
#define PRV_POWER_CURRENT_1   125
#define PRV_POWER_CURRENT_2   900
#define PRV_BATTERY_LEVEL     100
#define PRV_MEMORY_FREE       15
#define PRV_ERROR_CODE        0
#define PRV_TIME_ZONE         "Europe/Berlin"
#define PRV_BINDING_MODE      "U"

#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128

// Resource Id's:
#define RES_O_MANUFACTURER          0
#define RES_O_MODEL_NUMBER          1
#define RES_O_SERIAL_NUMBER         2
#define RES_O_FIRMWARE_VERSION      3
#define RES_M_REBOOT                4
#define RES_O_FACTORY_RESET         5
#define RES_O_AVL_POWER_SOURCES     6
#define RES_O_POWER_SOURCE_VOLTAGE  7
#define RES_O_POWER_SOURCE_CURRENT  8
#define RES_O_BATTERY_LEVEL         9
#define RES_O_MEMORY_FREE           10
#define RES_M_ERROR_CODE            11
#define RES_O_RESET_ERROR_CODE      12
#define RES_O_CURRENT_TIME          13
#define RES_O_UTC_OFFSET            14
#define RES_O_TIMEZONE              15
#define RES_M_BINDING_MODES         16
#define RES_O_DEVICE_TYPE           17
#define RES_O_HARDWARE_VERSION      18
#define RES_O_SOFTWARE_VERSION      19
#define RES_O_BATTERY_STATUS        20
#define RES_O_MEMORY_TOTAL          21

class ObjectDevice {
private:
    lwm2m_object_t * m_deviceObj;
public:
    ObjectDevice();

    void free_object_device(lwm2m_object_t * objectP);
    lwm2m_object_t * get_object_device();
    static uint8_t prv_device_read(uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP);
    static uint8_t prv_set_value(lwm2m_data_t * dataP);
    static uint8_t prv_device_execute(uint16_t instanceId,
                                      uint16_t resourceId,
                                      uint8_t * buffer,
                                      int length,
                                      lwm2m_object_t * objectP);
    static uint8_t prv_device_discover(uint16_t instanceId,
                                       int * numDataP,
                                       lwm2m_data_t ** dataArrayP,
                                       lwm2m_object_t * objectP);




};


#endif //IOT2PHONE_OBJECTDEVICE_H
