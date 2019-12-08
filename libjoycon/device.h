#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdio.h>
#include <string.h>

#pragma pack(1)

typedef struct MacAddress
{
    uint8_t _0;
    uint8_t _1;
    uint8_t _2;
    uint8_t _3;
    uint8_t _4;
    uint8_t _5;
} MacAddress_t;

typedef struct Alias
{
    char raw[20];
} Alias_t;

#pragma pack()

#define mac_address_le(address) ({MacAddress_t mac = {};sscanf(address, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac._5, &mac._4, &mac._3, &mac._2, &mac._1, &mac._0);mac; })
#define mac_address_be(address) ({MacAddress_t mac = {};sscanf(address, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac._0, &mac._1, &mac._2, &mac._3, &mac._4, &mac._5);mac; })
#define alias(str) ({Alias_t a = {};strcpy(a.raw,str);a; })

typedef struct Device
{
    const char *mac_address;
    const char *name;
} Device_t;

static Device_t SwitchConsole = {
    .name = "Nintendo Switch",
    .mac_address = "DC:68:EB:15:9A:62",
};

static Device_t JoyCon_L = {
    .name = "Joy-Con (L)",
    .mac_address = "",
};

static Device_t JoyCon_R = {
    .name = "Joy-Con (R)",
    .mac_address = "DC:68:EB:30:7F:08",
};

static Device_t ProController = {
    .name = "Pro Controller",
    .mac_address = "",
};

#endif //  _DEVICE_H_
