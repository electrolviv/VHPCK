/*
 * lib_huffman.h
 * Created on: Feb 19, 2016, www.electro-lviv.com
 * Author: Viktor Glebov
 * Rev V0.1
 */

#ifndef LIB_HUFFMAN_H_
#define LIB_HUFFMAN_H_

// Replace with your own system-depending header
#include "vhboard.h"

// Uncomment for enabling encoder, extra ~3K SRAM vars
#define VHPCK_USING_ENCODER 1

// Predefined variables types
/* #define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int32
#define verr u32
#define verror(X) X
#define vok 0
#define cu8 const u8 */

class VHPCK {

	public:
		typedef struct { u32 pfx; u16 leno; u16 lenp; } stHDR;

	private:
		typedef struct { u8 *ptr; u8 msk; } stBITP;
		stBITP wr,rd,tr; u16 values[512]; u16 left[512]; u16 right[512]; u16 cnt; u16 nodes; u16 ptrlim;

#ifdef VHPCK_USING_ENCODER
		u16 up[512]; u16 refs[256]; u8 backref[256]; u8 mrk[512];
		u16 FindMin() { u16 min=0xFFFF, r=0xFFFF; for(u16 i=0;i<cnt;i++) { if(!mrk[i]) if(values[i]<min) {min=values[i];r= i;}} return r;}
		void SPCNew(u8 sym) {refs[sym] = cnt; backref[cnt] = sym; values[cnt] = 0; mrk[cnt] = 0; NSLR(cnt,0xFFFF,0xFFFF); cnt++; nodes++; }
		void SPCInc(u8 sym) {if(refs[sym] == 0xFFFF) { SPCNew(sym); } values[refs[sym]]++; }
		void NLnk() { u16 idx1=FindMin(); mrk[idx1]=1; u16 idx2=FindMin(); mrk[idx2]=1;
		values[cnt]=values[idx1]+values[idx2];NSLR(cnt,idx1,idx2);
		mrk[cnt] = 0; up[idx1] = cnt; up[idx2] = cnt; cnt++; }
		u8 Path(u16 i) { u16 cur = i; u8 lenb = 0; SetPtr(&tr,mrk); while(cur != (cnt-1)) { PBit(&tr, left[up[cur]]==cur); lenb++; cur = up[cur]; } DecPtr(&tr); return lenb; }
		void HDRFin(stHDR *p, u16 o, u16 c) { p->pfx = 0x31484856; p->leno = o; p->lenp = c; } // Store header, 0x56484831 is VHH1
#endif

		void SetPtr(stBITP *p, u8 *pa) {p->ptr=pa;p->msk=0x80;}
		void IncPtr(stBITP *p) {if(p->msk==0x01) {p->ptr++;p->msk=0x80;} else p->msk>>=1;}
		void DecPtr(stBITP *p) {if(p->msk==0x80) {p->ptr--;p->msk=0x01;} else p->msk<<=1;}
		void PBit(stBITP *p,u8 b) { (*p->ptr)=b?((*p->ptr)|p->msk):((*p->ptr)&(~p->msk));IncPtr(p); }
		void PByte(stBITP *p,u8 v) { u8 m=0x80; while(m) {PBit(p,v&m);m>>=1;}}
		u8 TBit(stBITP *p) { return (*p->ptr) & p->msk ? 1:0; }
		u8 GBit(stBITP *p) { u8 r = TBit(p); IncPtr(p); return r; }
		u8 GByte(stBITP *p) { u8 r = 0, c=8; while(c--) { r<<=1; r|=GBit(p); } return r; }
		void NSLR(int i, u16 l, u16 r) {left[i]=l;right[i]=r;}
		void SPCClr() { for(u16 i=0;i<512;i++) values[i]=0xFFFF; } // Clear spectrum

	public:

#ifdef VHPCK_USING_ENCODER
		verr Encode(cu8 *src, u16 len, u8 *pResult, u16 lim) {
			cnt = 0; nodes = 0; SetPtr(&wr,pResult + sizeof(stHDR)); ptrlim = lim; // bitpointer setup
			for(u16 i=0;i<256;i++) { refs[i] = 0xFFFF; }
			SPCClr(); for(u16 i=0;i<len;i++) { SPCInc(src[i]);} // Create spectrum
			u16 symcount = cnt; while(nodes-->=2) {  NLnk(); } // Link tree
			PByte(&wr,symcount); // Store tree, chars count, sym store path and descriptor term
			for(u16 i=0;i<symcount;i++) { PByte(&wr,backref[i]); u8 lenb=Path(i); while(lenb--) {PBit(&wr,1);PBit(&wr,TBit(&tr));DecPtr(&tr);} PBit(&wr,0);}
			for(u16 i=0;i<len;i++) { u8 lenb = Path(refs[src[i]]); while(lenb--) { PBit( &wr, TBit(&tr)); DecPtr(&tr); } } // Store: Data, rev, d-path ... Save r-path
			HDRFin((stHDR *)pResult,len,wr.ptr-pResult+(wr.msk==0x80?0:1)-sizeof(stHDR));
			return vok;
		}
#endif

		verr Decode(cu8 *src, u8 *pResult, u16 lim) {
			stHDR *phdr = (stHDR *)src;
			cnt = 0; nodes = 0; SetPtr(&rd, (u8 *)src + sizeof(stHDR));
			ptrlim = lim; SPCClr(); NSLR(cnt,0xFFFF,0xFFFF); cnt++; // Mark root
			u16 symcnt = GByte(&rd);
			for(u16 i=0;i<symcnt;i++) { // Restore spectrum
				u8 sym = GByte(&rd); u16 cur = 0;
				while(GBit(&rd)) { u16 *p=GBit(&rd)?&right[cur]:&left[cur]; if(*p!=0xFFFF) {cur=*p;} else {*p=cnt;NSLR(cnt,0xFFFF,0xFFFF);cur=cnt;cnt++;}}
				values[cur] = sym; }
			u16 unpacked = 0, cur = 0; // Restore datastream, using header
			while(1) { cur = !GBit(&rd)?left[cur]:right[cur]; // Endpoint ?
				if(values[cur]!=0xFFFF) { if(unpacked>=lim) break;  *pResult = (u8)values[cur] & 0xFF; cur = 0; pResult++; unpacked++; if(unpacked == phdr->leno) break; }
			}
			return (phdr->leno == unpacked) ? vok : verror(1);
		}
};

#endif /* LIB_HUFFMAN_H_ */
