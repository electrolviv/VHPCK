#ifndef VHPCK21_HPP
#define VHPCK21_HPP

#include "vhpck21defs.hpp"



namespace VHPCK {

//  Maximal depth: 127 elements


class VHPCK21 {

    public:

        struct stHDR { u32 pfx; u16 leno; u16 lenp; };
        typedef enum { vOk=0, errWrongHDR, errWrongCRC, errSrc } enVHPCKErr;

        VHPCK21(u8 * pmemblock) {
            void *ptrs[] = { &tblval, &tbllef, &tblrig, &tblmrk, &tblup, &tbllooki, &tbllookc };
            for(int i=0; i < Defs::memblks_pck; i++) {
                *(void **)ptrs[i] = (void *)pmemblock; pmemblock+=256; }
        }

        void LinkTree(u8 lcnt) {
            for(;lcnt-- >= 2;) NLnk();
        }

        // Store tree, chars count, sym store path and descriptor term
        void StoreTree(u8 symcount, BitPtr *pwr) {
            BitPtr tr(bitpath);
            pwr->Put8(symcount);
            for(u8 i=0;i<symcount;i++) {
                u8 lenb=Path(i, &tr); pwr->Put8(tbllookc[i]);
                while(lenb--) {pwr->Put(1);tr.Dec();pwr->Put(tr.TBit());}
                pwr->Put(0); }
        }

        // Store: Data, rev, d-path ... Save r-path
        void StoreData(u8 *psrc, u8 len, BitPtr *pwr) {
            BitPtr tr(bitpath);
            for(u8 i=0;i<len;i++) {
                u8 lenb = Path(tbllooki[*psrc], &tr); psrc++;
                while(lenb--) { tr.Dec(); pwr->Put(tr.TBit()); }
            }
        }

        //
        verr EncodeChunk(BitPtr *pwr, u8 *src, u8 datalen) {
            u8 len = MakeSpectrum(src, datalen);
            u8 symcount = elms;
            LinkTree(elms);
            StoreTree(symcount, pwr);
            StoreData(src, len, pwr);
            return vOk;
        }


    private:

        // 256
        u8 *tblval; u8 *tbllef; u8 *tblrig; u8 *tblup;
        u8 *tblmrk; u8 *tbllooki; u8 *tbllookc;
        // Lookup table [256] char > idx
        // Lookup table [256] idx > char
        u8 elms;
        u8 bitpath[64];


        void fill(u8 *ptbl, u16 cnt, u8 val) { for(u16 i=0;i<cnt;i++) *ptbl++=val; }

        u8 MakeSpectrum(u8 *psrc, u8 len) {

            elms = 0;
            fill(tbllooki, 256, Defs::inv);
            fill(tblmrk, 256, 0);

            for(u8 i=0;i<len;i++) {
                if(tbllooki[*psrc] == Defs::inv) {
                    if(elms==Defs::elmslast) return i;
                    tblval[elms] = 1; tblmrk[elms] = 1;
                    tbllookc[elms] = *psrc;
                    tbllooki[*psrc] = elms++;
                } else {
                    tblval[tbllooki[*psrc]]++;
                }
                psrc++;
            }

            return len;
        }

        void FindMinPair(u8 *pidx1, u8 *pidx2) {

            u8 min, r;

            min=Defs::inv; r=Defs::inv;
            for(u8 i=0;i<elms;i++)
                if(tblmrk[i])
                    if(tblval[i]<min)
                        { min=tblval[i];r=i;}
            *pidx1 = r; tblmrk[r] = 0;

            min=Defs::inv; r=Defs::inv;
            for(u8 i=0;i<elms;i++)
                if(tblmrk[i])
                    if(tblval[i]<min)
                        { min=tblval[i];r=i;}
            *pidx2 = r; tblmrk[r] = 0;
        }


        void NLnk() {
            u8 idx1, idx2;
            FindMinPair(&idx1, &idx2);
            tblup[idx1] = elms; tblup[idx2] = elms;
            tbllef[elms] = idx1; tblrig[elms] = idx2;
            tblval[elms] = tblval[idx1] + tblval[idx2];
            tblmrk[elms] = 1;
            elms++; }

        u8 Path(u8 i, BitPtr *bp) {
            u8 cur = i; u8 lenb = 0;
            while(cur != (elms-1)) { bp->Put(tbllef[tblup[cur]]==cur); cur = tblup[cur]; lenb++; }
            return lenb; }

    public:

        void debug_block(const char *pfx, u8 *ptr, u16 len) {
            printf("%s\n", pfx);
            for(int i=0; i<len;) {
                i++; printf("%.2X", *ptr++);
                if( !(i%16) || (i==len) ) printf("\n");
                else if(i!=len) printf(" "); }
        }

        void debug_lookupi() { debug_block("LOOKUP idx", tbllooki, 256); }
        void debug_lookupc() { debug_block("LOOKUP chr", tbllookc, 256); }
        void debug_spk() { debug_block("SPK", tblval, elms); }
        void debug_left() { debug_block("LEFT :", tbllef, 256); }
        void debug_righ() { debug_block("RIGH :", tblrig, 256); }
        void debug_up() { debug_block("UP   :", tblup,  256); }

        void debug_idx(u8 idx) {
            printf("#%i LEFT:%d RIGHT:%d UP:%d\n",
                   idx, tbllef[idx], tblrig[idx], tblup[idx]);
        }

        void debug_ptrs() {
            printf("Pointer tblval %p %p\n", tblval, &tblval);
            printf("Pointer tbllef %p %p\n", tbllef, &tbllef);
            printf("Pointer tblrig %p %p\n", tblrig, &tblrig);
            fflush(stdout);
        }

};

}


#endif // VHPCK21_HPP


/*

void ClrSpk()  { fill(values, 512, 0xFFFF); }
void ClrRefs() { fill(refs, 256, 0xFFFF); }

verr CheckHDR(void *p) {return ((stHDR *)p)->pfx==VHPCK_HDR?vok:errWrongHDR;}
u16 OSize(void *p) {return ((stHDR *)p)->leno;} // Uncompressed data length
u16 PSize(void *p) {return ((stHDR *)p)->lenp;} // Packed data length
*/

// HDRFin(pResult,len,wr.ptr-pResult+(wr.msk==0x80?0:1));

// debug_spk();
// debug_left();
// debug_righ();
// debug_up();
// debug_lookup();
// for(int i=0; i<elms; i++) debug_idx(i);

/*
void SPCNew(u8 sym) {
    refs[sym] = cnt;
    backref[cnt] = sym;
    values[cnt] = 0;
    mrk[cnt] = 0;
    NSLR(cnt++,0xFFFF,0xFFFF);
    nodes++; }

void SPCInc(u8 sym) {if(refs[sym] == 0xFFFF) { SPCNew(sym); } values[refs[sym]]++; }

// Store header, 0x56484832 is VHH2
void HDRFin(u8 *ptr, u16 o, u16 c) {
    stHDR * p = (stHDR *)ptr; p->pfx = VHPCK_HDR; p->leno = o; p->lenp = c; }

*/
