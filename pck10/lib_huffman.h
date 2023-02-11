/*
 * lib_huffman.h
 * Created on: Feb 19, 2016, www.electro-lviv.com
 * Updated on: Feb 10, 2023
 * Author: Viktor Glebov
 * Rev #0.2
 */

#ifndef LIB_HUFFMAN_H_
#define LIB_HUFFMAN_H_

#include "lib_huffman_defs.h"
#include "lib_huffman_bptr.h"

class VHPCK {
	private:
        const u16 inv = 0xFFFF;
        VHBptr wr,rd,tr; u16 vals[512], left[512], right[512], cnt, nodes;
        void NSLR(int i, u16 l, u16 r) {left[i]=l;right[i]=r;}
        void ClrSpc() { for(u16 i=0;i<512;i++) vals[i]=inv; } // Clear spectrum
	public:
        struct stHDR { u32 pfx; u16 leno; u16 lenp; }; typedef enum { vOk=0, errWrongHDR, errWrongCRC, errSrc } enVHPCKErr;
#ifdef VHPCK_USING_ENCODER
        u16 up[512]; u16 refs[256]; u8 backref[256]; u8 mrk[512];
        void FindAny(u16 *pidx) { for(;*pidx<cnt;(*pidx)++) if(!mrk[*pidx]) { return; } }
        void Find2Min(u16 *r1, u16 *r2) { u16 idx = 0, t, min1, min2;
            FindAny(&idx); min1=vals[idx]; *r1=idx++; FindAny(&idx); min2=vals[idx]; *r2=idx++;
            if(min2<min1) {t=min2; min2=min1; min1=t; t=*r1; *r1=*r2; *r2=t;} // swap: min2 should be always > min1
            for(;idx<cnt;idx++) if(!mrk[idx]) { if(vals[idx]<min1) {*r2=*r1;min2=min1;*r1=idx;min1=vals[idx];}
            else if(vals[idx]<min2) {*r2=idx;min2=vals[idx];}}}
        void SPCNew(u8 sym) {refs[sym]=cnt; backref[cnt]=sym; vals[cnt]=0; mrk[cnt]=0; NSLR(cnt,inv,inv); cnt++; nodes++; }
        void ClrRefs() { for(u16 i=0;i<256;i++) {refs[i]=inv;}}
        void SPCInc(u8 sym) {if(refs[sym]==inv) {SPCNew(sym);} vals[refs[sym]]++;}
        void SPCCalc(u8 *src, u16 len) { for(u16 i=0;i<len;i++) {SPCInc(src[i]);}} // Create spectrum
        void NLnk() { u16 idx1, idx2; Find2Min(&idx1, &idx2); mrk[idx1]=1; mrk[idx2]=1;
            vals[cnt]=vals[idx1]+vals[idx2]; NSLR(cnt,idx1,idx2); mrk[cnt]=0; up[idx1]=cnt; up[idx2]=cnt; cnt++; }
        u8 Path(u16 i) { u16 cur=i; u8 lenb=0; tr.set(mrk);
            while(cur!=(cnt-1)) { tr.PBit(left[up[cur]]==cur); lenb++; cur=up[cur]; } tr.dec(); return lenb; }
        void HDRFin(stHDR *p, u16 o, u16 c) { p->pfx=VHPCK_HDR; p->leno=o; p->lenp=c; } // Store header, 0x56484831 is VHH1
		verr Encode(cvoid *psrc, u16 len, u8 *pResult, u16 lim) {
            u8 *src=(u8 *)psrc; cnt=0; nodes=0; wr.set(pResult + sizeof(stHDR)); // bitpointer setup
            ClrRefs(); ClrSpc(); SPCCalc(src, len); u16 symcount = cnt; while(nodes-->=2) {  NLnk(); } // Link tree
            wr.PByte(symcount); // Store tree, chars count, sym store path and descriptor term, Store: Data, rev, d-path ... Save r-path
            for(u16 i=0;i<symcount;i++) { wr.PByte(backref[i]); u8 lenb=Path(i); while(lenb--) {wr.PBit1();wr.PBit(tr.TBit());tr.dec();} wr.PBit0();}
            for(u16 i=0;i<len;i++) { u8 lenb=Path(refs[src[i]]); while(lenb--) { wr.PBit(tr.TBit()); tr.dec(); } }
            HDRFin((stHDR *)pResult,len,wr.bytescnt()); return vOk; }
#endif
		verr CheckHDR(void *p) {return ((stHDR *)p)->pfx==VHPCK_HDR?vok:errWrongHDR;}
		u16 OSize(void *p) {return ((stHDR *)p)->leno;} // Uncompressed data length
		u16 PSize(void *p) {return ((stHDR *)p)->lenp;} // Packed data length
        verr Decode(cvoid *psrc, void *pResult, u16 lim) { u8 *pRes=(u8 *)pResult; stHDR *phdr=(stHDR *)psrc; cnt=0; nodes=0;
            rd.set((u8 *)psrc + sizeof(stHDR)); ClrSpc(); NSLR(cnt,inv,inv); cnt++; // Mark root
            u16 symcnt = rd.GByte(); for(u16 i=0;i<symcnt;i++) { u8 sym=rd.GByte(); u16 cur=0; // Restore spectrum
            while(rd.GBit()) { u16 *p=rd.GBit()?&right[cur]:&left[cur]; if(*p!=inv) {cur=*p;} else {*p=cnt;NSLR(cnt,inv,inv);cur=cnt;cnt++;}}
            vals[cur]=sym; } u16 unpacked=0, cur=0; while(1) { cur=!rd.GBit()?left[cur]:right[cur]; // Restore datastream, Endpoint ?
            if(vals[cur]!=inv) { if(unpacked>=lim) break; *pRes=(u8)vals[cur]&0xFF; cur=0; pRes++; unpacked++; if(unpacked==phdr->leno) break; } }
            return (phdr->leno==unpacked)?vOk:errSrc; }
};

#endif /* LIB_HUFFMAN_H_ */
