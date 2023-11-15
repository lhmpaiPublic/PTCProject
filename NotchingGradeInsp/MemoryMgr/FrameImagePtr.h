#pragma once

#include "MemoryPool.h"
#define MAX_FRAMEINFOPOOL (4000*14000)+1
class FrameImagePtr : public CMemoryPool<FrameImagePtr,10>
{
public:
    __int64  addressStart;
    BYTE	m_pImagePtr[MAX_FRAMEINFOPOOL];
};