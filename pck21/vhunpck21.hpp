/*
 * (C) 2000-2020 V01G04A81
 */

#ifndef VHUNPCK21_HPP
#define VHUNPCK21_HPP

#if defined (__linux__) || (__GNUC__)

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Predefined variables types
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define verr u32
#define verror(X) X
#define vok 0
#define cu8 const u8
#define cvoid const void

#elif _WIN32
#error "Not supported at the moment"
#endif


namespace VHPCK {

#define     VHPCK_CHNK_MAX      254
#define     VHPCK_ELMS_MAX      64
#define     VHPCK_SAME_MAX      6
#define     VHPCK_HDR21         0x31484857

struct stPckHdr  { u32 pfs; u32 os; u32 ps; u32 crc; };


class VHUNPCK21 {

public:

    typedef u8 (*funcget)(void);
    typedef void (*funcput)(u8);
    static const u8 inv = 255;

    VHUNPCK21(u8 * pmemblock, funcget ifcget, funcput ifcput) {
        _gb = ifcget; _pb = ifcput;
        void *ptrs[] = { &tblval, &tbllef, &tblrig };
        for(size_t i=0; i < sizeof(ptrs) / sizeof(void*); i++) {
            *(void **)ptrs[i] = (void *)pmemblock; pmemblock+=256; } }

    u8 GBit()  { if(!cm) { cm=0x80; cv=_gb(); } u8 r=cv&cm?1:0; cm>>=1; return r; }
    u8 GByte() { u8 r = 0; for(u8 i=0;i<8;i++) { r<<=1; r|=GBit(); } return r; }
    void rst(u8 idx) { tbllef[idx]=inv; tblrig[idx]=inv; tblval[idx]=inv; }

    void DecodeChunk(u8 dstlen) {
        u8 symcnt = GByte(), pos = 0, cnt = 0, elms = 1; rst(0);
        for(u8 i=0;i<symcnt;i++) { u8 sym = GByte(), cur = 0;
            while(GBit()) { u8 *p=GBit()?&tblrig[cur]:&tbllef[cur];
                if(*p==inv) { *p=elms; rst(elms); cur=elms++;}
                else {cur=*p;} } tblval[cur] = sym; }
        while(cnt != dstlen) { pos = ! GBit() ? tbllef[pos]:tblrig[pos];
            if(tbllef[pos]==inv) { _pb(tblval[pos]); pos = 0; cnt++; }}}

    void Decode() {
        cm = 0; for(;;) { u8 chkt = (GBit() << 1) + GBit();
            if (chkt == 1) { DecodeChunk(GByte()); }
            else if (chkt == 2) { u8 cnt = GByte();
                for(u8 i=0;i<cnt;i++) _pb(GByte()); }
            else if (chkt == 3) { u8 cnt = GByte(), sym = GByte();
                for(u8 i=0;i<cnt;i++) _pb(sym); } else break; } }

private:

    u8 *tblval; u8 *tbllef; u8 *tblrig;
    funcget _gb;
    funcput _pb;
    u8 cv, cm;

};

}

#endif // VHUNPCK21_HPP
