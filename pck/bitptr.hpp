#ifndef BITPTR_HPP
#define BITPTR_HPP

#include "vhplatform.h"

namespace VHPCK {


#define VHPCK_HDR   0x31484856
#define VHPCK_HDR2  0x31484857


class BitPtr {

public:
    BitPtr(u8 *ptr, u8 msk=0x80) { this->Set(ptr,msk); }
    void Set(u8 *ptr, u8 msk=0x80) { this->ptr = ptr; this->msk = msk; }
    void Inc() { if(msk==0x01) {ptr++;msk=0x80;} else msk>>=1; }
    void Dec() { if(msk==0x80) {ptr--;msk=0x01;} else msk<<=1; }
    void Put(u8 b) { (*ptr)=b?((*ptr)|msk):((*ptr)&(~msk)); Inc(); }
    void Put8(u8 v) { u8 m=0x80; while(m) { Put(v&m);m>>=1; }}
    u8 TBit() { return (*ptr)&msk?1:0;}
    u8 GBit() { u8 r = TBit(); Inc(); return r; }
    u8 GByte() { u8 r = 0, c=8; while(c--) { r<<=1; r|=GBit(); } return r; }

private:
    u8 *ptr; u8 msk;

};


}


#endif // BITPTR_HPP
