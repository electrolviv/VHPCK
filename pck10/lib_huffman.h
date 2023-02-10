/*
 * lib_huffman.h
 * Created on: Feb 19, 2016, www.electro-lviv.com
 * Author: Viktor Glebov
 * Rev V0.1
 */

#ifndef LIB_HUFFMAN_H_
#define LIB_HUFFMAN_H_

#include "lib_huffman_defs.h"

class VHPCK {

	public:
        struct stHDR { u32 pfx; u16 leno; u16 lenp; }; typedef enum { vOk=0, errWrongHDR, errWrongCRC, errSrc } enVHPCKErr;
	private:
        typedef struct { u8 *ptr; u16 bcnt; } sBPTR;
        sBPTR wr,rd,tr; u16 vals[512]; u16 left[512]; u16 right[512]; u16 cnt; u16 nodes; u16 ptrlim;
#ifdef VHPCK_USING_ENCODER
        u16 up[512]; u16 refs[256]; u8 backref[256]; u8 mrk[512];
#endif
        const u8 mask0[8] = { 0x7F, 0xBF, 0xDF, 0xEF,  0xF7, 0xFB, 0xFD, 0xFE };
        const u8 mask1[8] = { 0x80, 0x40, 0x20, 0x10,  0x08, 0x04, 0x02, 0x01 };

        void PtrS(sBPTR *p, u8 *pa) {p->ptr=pa;p->bcnt=0;}
        void PtrI(sBPTR *p) { p->bcnt++; }
        void PtrD(sBPTR *p) { p->bcnt--; }
        u8 * PtrA(sBPTR *p) { return p->ptr + (p->bcnt >> 3); }
        u8   PtrM0(sBPTR *p) { return mask0[p->bcnt & 0x7]; }
        u8   PtrM1(sBPTR *p) { return mask1[p->bcnt & 0x7]; }
        void PBit0(sBPTR *p) { *PtrA(p) &=  PtrM0(p); PtrI(p); }
        void PBit1(sBPTR *p) { *PtrA(p) |=  PtrM1(p); PtrI(p); }
        void PBit(sBPTR *p,u8 b) { if(b) PBit1(p); else PBit0(p); }
        void PByte(sBPTR *p,u8 v) { for(u8 m=0x80; m; m>>=1) { PBit(p,v&m); }}
        u8 TBit(sBPTR *p) { return *PtrA(p) & mask1[p->bcnt&0x7] ? 1:0; }
        u8 GBit(sBPTR *p) { u8 r = TBit(p); PtrI(p); return r; }
        u8 GByte(sBPTR *p) { u8 r = 0, c=8; while(c--) { r<<=1; r|=GBit(p); } return r; }

        void NSLR(int i, u16 l, u16 r) {left[i]=l;right[i]=r;}
        void ClrSpc() { for(u16 i=0;i<512;i++) vals[i]=0xFFFF; } // Clear spectrum

	public:

#ifdef VHPCK_USING_ENCODER

        bool Find1Min(u16 *pidx, u16 *pmin) { for(;*pidx<cnt;(*pidx)++) if((!mrk[*pidx])&&(vals[*pidx]<*pmin)) { *pmin = vals[*pidx]; return true; } return false; }

        void Find2Min(u16 *r1, u16 *r2) { u16 idx = 0, t, min1=0xFFFF, min2=0xFFFF;
            Find1Min(&idx, &min1); *r1 = idx++; Find1Min(&idx, &min2); *r2 = idx++;
            if(min2<min1) { t = min2; min2=min1; min1=t;  t=*r1; *r1=*r2; *r2=t; } // swap: min2 should be always > min1
            for(;idx<cnt;idx++) if(!mrk[idx]) { if(vals[idx]<min1) { *r2 = *r1; *r1 = idx; } else if(vals[idx]<min2) { *r2 = idx; } }
            /* printf("Find2Min() '%c' %d , '%c' %d\n", backref[*r1], vals[*r1], backref[*r2], vals[*r2] ); */ }

        void SPCNew(u8 sym) {refs[sym] = cnt; backref[cnt] = sym; vals[cnt] = 0; mrk[cnt] = 0; NSLR(cnt,0xFFFF,0xFFFF); cnt++; nodes++; }
        void ClrRefs() { for(u16 i=0;i<256;i++) { refs[i] = 0xFFFF; } }
        void SPCInc(u8 sym) {if(refs[sym] == 0xFFFF) { SPCNew(sym); } vals[refs[sym]]++; }
        void NLnk() { u16 idx1, idx2; Find2Min(&idx1, &idx2); mrk[idx1]=1; mrk[idx2]=1;
            vals[cnt]=vals[idx1]+vals[idx2]; NSLR(cnt,idx1,idx2);
            mrk[cnt] = 0; up[idx1] = cnt; up[idx2] = cnt; cnt++; }
        u8 Path(u16 i) { u16 cur = i; u8 lenb = 0; PtrS(&tr,mrk);
            while(cur != (cnt-1)) { PBit(&tr, left[up[cur]]==cur); lenb++; cur = up[cur]; } PtrD(&tr); return lenb; }
        void HDRFin(stHDR *p, u16 o, u16 c) { p->pfx = VHPCK_HDR; p->leno = o; p->lenp = c; } // Store header, 0x56484831 is VHH1
		verr Encode(cvoid *psrc, u16 len, u8 *pResult, u16 lim) {
            u8 *src =(u8 *)psrc; cnt = 0; nodes = 0; PtrS(&wr,pResult + sizeof(stHDR)); ptrlim = lim; // bitpointer setup
            ClrRefs(); ClrSpc(); for(u16 i=0;i<len;i++) { SPCInc(src[i]);} // Create spectrum
			u16 symcount = cnt; while(nodes-->=2) {  NLnk(); } // Link tree
			PByte(&wr,symcount); // Store tree, chars count, sym store path and descriptor term
            for(u16 i=0;i<symcount;i++) { PByte(&wr,backref[i]); u8 lenb=Path(i); /* printf("#%d (%c) lenb=%d\n", i, backref[i], lenb); */
                while(lenb--) {PBit1(&wr);PBit(&wr,TBit(&tr));PtrD(&tr);} PBit0(&wr);} // Store: Data, rev, d-path ... Save r-path
            for(u16 i=0;i<len;i++) { u8 lenb = Path(refs[src[i]]); while(lenb--) { PBit( &wr, TBit(&tr)); PtrD(&tr); } }
            HDRFin((stHDR *)pResult,len,(wr.bcnt>>3)+((wr.bcnt&7)?1:0));
            return vOk; }
#endif
		verr CheckHDR(void *p) {return ((stHDR *)p)->pfx==VHPCK_HDR?vok:errWrongHDR;}
		u16 OSize(void *p) {return ((stHDR *)p)->leno;} // Uncompressed data length
		u16 PSize(void *p) {return ((stHDR *)p)->lenp;} // Packed data length
		verr Decode(cvoid *psrc, void *pResult, u16 lim) {
            u8 *pRes = (u8 *)pResult; stHDR *phdr = (stHDR *)psrc; cnt = 0; nodes = 0; PtrS(&rd, (u8 *)psrc + sizeof(stHDR)); ptrlim = lim;
            ClrSpc(); NSLR(cnt,0xFFFF,0xFFFF); cnt++; // Mark root
            u16 symcnt = GByte(&rd); for(u16 i=0;i<symcnt;i++) { u8 sym = GByte(&rd); u16 cur = 0; // Restore spectrum
				while(GBit(&rd)) { u16 *p=GBit(&rd)?&right[cur]:&left[cur]; if(*p!=0xFFFF) {cur=*p;} else {*p=cnt;NSLR(cnt,0xFFFF,0xFFFF);cur=cnt;cnt++;}}
                vals[cur] = sym; }
            u16 unpacked = 0, cur = 0; // Restore datastream
			while(1) { cur = !GBit(&rd)?left[cur]:right[cur]; // Endpoint ?
                if(vals[cur]!=0xFFFF) { if(unpacked>=lim) break;  *pRes = (u8)vals[cur] & 0xFF; cur = 0; pRes++; unpacked++; if(unpacked == phdr->leno) break; } }
            return (phdr->leno==unpacked)?vOk:errSrc; }
};

#endif /* LIB_HUFFMAN_H_ */
