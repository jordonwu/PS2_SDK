#ifdef __cplusplus
extern "C" {
#endif

#include "kernel.h"
#include "libpad.h"
#define ROM_PADMAN

#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

extern char actAlign[6];
extern int actuators;
extern struct padButtonStatus buttons;
extern int controllerReturn;

extern unsigned int paddata;
extern unsigned int old_pad;
extern unsigned int new_pad;

int waitPadReady(int port, int slot);
int initializePad(int port, int slot);
void CheckConnection(int connect);

#ifdef __cplusplus
}
#endif
