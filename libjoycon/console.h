#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "device.h"
#include "session.h"

int Console_establish(Session_t *);
int Console_abolish(Session_t *);
int Console_getControllerInfo(Session_t *);
int Console_getControllerColor(Session_t *);
int Console_setPlayerLight(Session_t *, Player_t, PlayerFlash_t);
int Console_setHomeLight(Session_t *);

#endif //  _CONSOLE_H_
