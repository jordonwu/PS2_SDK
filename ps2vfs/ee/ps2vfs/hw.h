// hw.h

#ifndef _HW_H_
#define _HW_H_

#ifdef __cplusplus
extern "C" {
#endif


/* not too useful */
void install_VRend_handler();
void remove_VRend_handler();
void WaitForNextVRend();
int TestVRend();
void ClearVRendf();

/* quite useful */
void install_VRstart_handler();
void remove_VRstart_handler();
void WaitForNextVRstart(int numvrs);
int TestVRstart();
void ClearVRcount();

void initGraph(int mode);
void SetVideoMode();
void SetDrawFrameBuffer(int which);
void SetCrtFrameBuffer(int which);

void DmaReset();
void SendDma02(void *DmaTag);
void Dma02Wait();

void DCacheFlush();

void resetVU0();

void qmemcpy(void *dest, void *src, int numqwords);
void dmemcpy(void *dest, void *src, int numdwords);
void wmemcpy(void *dest, void *src, int numwords);

#ifdef __cplusplus
}
#endif

#endif
