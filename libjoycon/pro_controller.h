#ifndef _PRO_CONTROLLER_H_
#define _PRO_CONTROLLER_H_

#include "controller.h"

API_PUBLIC Controller_t *createProController(void *);
API_PUBLIC void releaseProController(Controller_t *);

#endif // !_PRO_CONTROLLER_H_
