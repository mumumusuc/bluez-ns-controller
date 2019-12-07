#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "pro_controller.h"

void dump_hex(uint8_t *data, size_t len);

int main()
{
    printf("hello world\n");

    Payload_t pro;
    memset(&pro, 0, sizeof(pro));
    init(&pro);
    pro.button.Y = PRESSED;
    pro.button.X = PRESSED;
    pro.button.L = PRESSED;
    pro.button.ZL = PRESSED;
    dump_hex((uint8_t *)&pro, sizeof(pro));

    return 0;
}

void dump_hex(uint8_t *data, size_t len)
{
    if (!data)
        return;
    for (int i = 0; i < len; i++)
        fprintf(stdout, "0x%02x ", data[i]);
    fprintf(stdout, "\n");
}
