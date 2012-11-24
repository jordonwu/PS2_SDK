
#ifndef _SPU2_H_
#define _SPU2_H_

// Param
#define SD_VPARAM_VOLL				(0x00<<8)
#define SD_VPARAM_VOLR				(0x01<<8)
#define SD_VPARAM_PITCH				(0x02<<8)
#define SD_VPARAM_ADSR1				(0x03<<8)
#define SD_VPARAM_ADSR2				(0x04<<8)
#define SD_VPARAM_ENVX				(0x05<<8)
#define SD_VPARAM_VOLXL				(0x06<<8)
#define SD_VPARAM_VOLXR				(0x07<<8)
#define SD_PARAM_MMIX				(0x08<<8)
#define SD_PARAM_MVOLL				((0x09<<8)|0x80)
#define SD_PARAM_MVOLR				((0x0A<<8)|0x80)
#define SD_PARAM_EVOLL				((0x0B<<8)|0x80)
#define SD_PARAM_EVOLR				((0x0C<<8)|0x80)
#define SD_PARAM_AVOLL				((0x0D<<8)|0x80)
#define SD_PARAM_AVOLR				((0x0E<<8)|0x80)
#define SD_PARAM_BVOLL				((0x0F<<8)|0x80)
#define SD_PARAM_BVOLR				((0x10<<8)|0x80)
#define SD_PARAM_MVOLXL				((0x11<<8)|0x80)
#define SD_PARAM_MVOLXR				((0x12<<8)|0x80) 

// Addr
#define SD_ADDR_ESA				(0x1C<<8)
#define SD_ADDR_EEA				(0x1D<<8)
#define SD_ADDR_TSA				(0x1E<<8)
#define SD_ADDR_IRQA				(0x1F<<8)

// SD_CORE_ATTR Macros
#define SD_SPU2_ON				(1 << 15)
#define SD_MUTE					(1 << 14)
#define SD_NOISE_CLOCK(c)			((c & 0x1F) << 8) // Bits 8..13 is noise clock
#define SD_ENABLE_EFFECTS			(1 << 7)
#define SD_ENABLE_IRQ				(1 << 6)
#define SD_DMA_IO				(1 << 4)
#define SD_DMA_WRITE				(2 << 4)
#define SD_DMA_READ				(3 << 4)
#define SD_DMA_IN_PROCESS			(3 << 4) // If either of the DMA bits are set, the DMA channel is occupied.
#define SD_CORE_DMA				(3 << 4)
#define SD_ENABLE_EX_INPUT			(1 << 0) // Enable external input, Not sure. 

// SD_C_STATX
#define SD_IO_IN_PROCESS			(1 << 10) 

// CoreAttr
#define SD_CORE_EFFECT_ENABLE			0x2
#define SD_CORE_IRQ_ENABLE			0x4
#define SD_CORE_MUTE_ENABLE			0x6
#define SD_CORE_NOISE_CLK			0x8
#define SD_CORE_SPDIF_MODE			0xA 

// BlockTrans
#define SD_BLOCK_TRANS_WRITE		0
#define SD_BLOCK_TRANS_READ			1
#define SD_BLOCK_TRANS_STOP			2
#define SD_BLOCK_TRANS_WRITE_FROM	3
#define SD_BLOCK_TRANS_LOOP			0x10 

typedef struct
{	
	u32 mode;
	void *data;
} IntrData;
 

typedef int (*SdSpu2IntrHandler)(int core, void *data);
typedef int (*SdTransIntrHandler)(int channel, void *data);
typedef int (*IntrCallback)(void *data); 

s32 SdInit(s32 flag);
void SdSetParam(u16 reg, u16 val);
void SdSetCoreAttr(u16 entry, u16 val);
IntrCallback SdSetTransCallback(int core, IntrCallback cb);
int SdBlockTrans(s16 chan, u16 mode, u8 *iopaddr, u32 size, u8 *startaddr);
u32 SdBlockTransStatus(s16 chan, s16 flag);

#endif
