#ifndef VHUNPCK21_HPP
#define VHUNPCK21_HPP

#include "vhpck21defs.hpp"


namespace VHPCK {

class VHUNPCK21 {

    public:

    VHUNPCK21(u8 * pmemblock) {
        void *ptrs[] = { &tblval, &tbllef, &tblrig };
        for(int i=0; i< Defs::memblks_unpck; i++) {
            *(void **)ptrs[i] = (void *)pmemblock; pmemblock+=256;
        }
    }

    verr DecodeChunk(u8 *psrc, u8 srclen, u8 *pRes) {

        BitPtr rd(psrc);

        tbllef[0] = Defs::inv;
        tblrig[0] = Defs::inv;
        tblval[0] = Defs::inv;
        elms = 1;

        u8 symcnt = rd.GByte();

        // Restore spectrum
        for(u8 i=0;i<symcnt;i++) {
            u8 sym = rd.GByte(); u8 cur = 0;
            printf("Sym 0x%x ", sym);
            while(rd.GBit()) {
                u8 dirbit = rd.GBit();
                printf("%d", dirbit);
                u8 *p=dirbit?&tblrig[cur]:&tbllef[cur];
                if(*p==Defs::inv) {
                    *p=elms;
                    tbllef[elms]=Defs::inv;
                    tblrig[elms]=Defs::inv;
                    tblval[elms]=Defs::inv;
                    cur=elms++;}
                else {cur=*p;}

            }
            printf("\n");
            tblval[cur] = sym;
        }

        for(int i=0;i<10;i++)
            debug_idx(i);

        // Restore datastream, using header
        u8 cur = 0;
        for(u8 unpacked = 0; unpacked != srclen;) {
            cur = ! rd.GBit() ? tbllef[cur]:tblrig[cur];
            if(tblval[cur]!=Defs::inv) { *pRes++ = tblval[cur]; cur = 0; unpacked++; }
        }

        // return (phdr->leno==unpacked)?vOk:errSrc;

        return vok;
    }

    private:

        u8 *tblval; u8 *tbllef; u8 *tblrig;
        u8 elms;

        void debug_idx(u8 idx) {
            printf("#%i LEFT:%d RIGHT:%d VAL:%d\n", idx, tbllef[idx], tblrig[idx], tblval[idx]);
        }



};

}

#endif // VHUNPCK21_HPP
