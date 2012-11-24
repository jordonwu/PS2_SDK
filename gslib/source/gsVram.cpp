#include <tamtypes.h>
#include "gsDefs.h"
#include "gsVram.h"


gsVram::gsVram(gsDriver& inDriver)
{
    m_vramPointer = inDriver.getTextureBufferBase();
}

void gsVram::setPointer(u32 inPointer)
{
    m_vramPointer = inPointer;
}

// get amount of vram available
u32 gsVram::getAvailable()
{
    return (kVramMax - m_vramPointer);
}

// get current vram used
u32 gsVram::getCurrentPointer()
{
    return m_vramPointer;
}

u32 gsVram::getTotal()
{
    return kVramMax;
}

#define GS_VRAM_TBWALIGN_CLUT 128
#define GS_VRAM_TBWALIGN 64

// get pointer to unallocated vram space for a texture
u32 gsVram::getTBP(int inTexWidth, int inTexHeight, int inPSM)
{
    u32 size = 0;

    // first setup the dimensions depending on pixel storage method
    switch (inPSM) {
        case GS_PSMT4:
        case GS_PSMT8:
            inTexWidth = (-GS_VRAM_TBWALIGN_CLUT)&(inTexWidth+GS_VRAM_TBWALIGN_CLUT-1);
            break;
        case GS_PSMCT16:
        case GS_PSMCT16S:
        case GS_PSMCT24:
        case GS_PSMCT32:
            inTexWidth = (-GS_VRAM_TBWALIGN)&(inTexWidth+GS_VRAM_TBWALIGN-1);
            break;
    }

    inTexHeight = (-GS_VRAM_TBWALIGN)&(inTexHeight+GS_VRAM_TBWALIGN-1);

    switch(inPSM) {
        case GS_PSMCT32:  size = (inTexWidth*inTexHeight*4); break;
        case GS_PSMCT24:  size = (inTexWidth*inTexHeight*4); break;
        case GS_PSMCT16:  size = (inTexWidth*inTexHeight*2); break;
        case GS_PSMCT16S: size = (inTexWidth*inTexHeight*2); break;
        case GS_PSMT8:    size = (inTexWidth*inTexHeight  ); break;
        case GS_PSMT4:    size = (inTexWidth*inTexHeight/2); break;
	default: return 0;
    }

    // then modify size to make sure it's properly aligned
    size = (-256)&(size+256-1);

    if(m_vramPointer + size >= kVramMax)
        return 0;

    m_vramPointer += size;

    // return starting point of tbp
    return (m_vramPointer-size);
}

// reset pointer to beginning of useable vram space
void gsVram::Reset(gsDriver& inDriver)
{
	m_vramPointer = inDriver.getTextureBufferBase();
}

