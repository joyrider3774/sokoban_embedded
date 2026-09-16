#ifndef CVIEWPORT_H
#define CVIEWPORT_H

#include <stdint.h>

typedef struct CViewPort CViewPort;
//Everything here is 16 bit so the game can run on a bigger screen: the pixel values
//grow with the resolution, and so do the tile values, as they include the number of
//tiles visible (VPLimitMinX + NrOfColsVisible)
struct CViewPort
{
    //signed: SetViewPort stores a position left of / above the level before clamping it
    int16_t VPMinX,VPMinY,VPMaxX,VPMaxY,MinScreenX,MinScreenY,MaxScreenX,MaxScreenY;
    //never negative, SetVPLimit only stores limits of 0 and up
    uint16_t Width,Height,VPLimitMinX,VPLimitMaxX,VPLimitMinY,VPLimitMaxY;
};

CViewPort* CViewPort_Create(int16_t MinX,int16_t MinY, int16_t MaxX,int16_t MaxY,int16_t MinX2,int16_t MinY2,int16_t MaxX2,int16_t MaxY2);
void CViewPort_SetVPLimit(CViewPort* ViewPort, int16_t MinX,int16_t MinY, int16_t MaxX,int16_t MaxY);
void CViewPort_Move(CViewPort* ViewPort, int8_t Xi,int8_t Yi);
void CViewPort_SetViewPort(CViewPort* ViewPort, int16_t MinX,int16_t MinY, int16_t MaxX,int16_t MaxY);
void CViewPort_Destroy(CViewPort* ViewPort);

#endif