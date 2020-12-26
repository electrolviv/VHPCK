/*
 * pck_spacer.c
 *
 *  Created on: Feb 24, 2016
 *      Author: Viktor
 */

#include "pck_spacer.h"

int Pack(u8 *src_dat, int src_len, u8 *result, int res_len) {

	u8 *ptr_src = src_dat;
	u8 *ptr_pfx = result + sizeof(int);
	u8 *ptr_sav = ptr_pfx+1;
	int dubs;

	* ((int *)result) = src_len;

	while(1) {

		// Null prefix
		*ptr_pfx = 0;

		// Collect dat
		while(1) {
			dubs = 1;

			if(!(*ptr_pfx)) { // Zero
					(*ptr_pfx)++; *ptr_sav = *ptr_src; ptr_src++; src_len--; ptr_sav++; if(!src_len) break;
			} else { // Non-zero
				if( *(ptr_src) != *(ptr_src-1)) {
					(*ptr_pfx)++; *ptr_sav = *ptr_src; ptr_src++; src_len--; ptr_sav++; if(!src_len) break;
				} else {
					ptr_src++; src_len--; dubs = 2; break;
				}

				if((*ptr_pfx)>=250) { break; }
			}
		}

		if(dubs>1) { (*ptr_pfx)--; ptr_sav--; }
		if(src_len) { while(1) { if( *ptr_src == *(ptr_src-1) ) { ptr_src++; src_len--; dubs++; if((!src_len) || (dubs>=254)) break; } else { break; } } } // Collect dub
		*ptr_sav = dubs; ptr_sav++; *ptr_sav = *(ptr_src-1); ptr_sav++; ptr_pfx = ptr_sav; ptr_sav++; // Save dubs
		if(!src_len) break;
	}

	return ptr_pfx-result;
}

enPCKStatus VHPCK_UnpackNext(stPCKSpacer *ppck) {

	if(ppck->firstStart) {
		ppck->firstStart = 0;
		ppck->rBytes = *((int *)ppck->src);
		ppck->src += 4; ppck->len -= 4;
	}

	while(1) {

		// Data transfer
		while(ppck->store_dat_cnt) {
			if(!ppck->len) return ePCK_InBuffer;
			if(!ppck->lim) return ePCK_OutBuffer;
			*(ppck->out) = *(ppck->src);
			ppck->out++; ppck->src++;
			ppck->len--; ppck->lim--;
			ppck->store_dat_cnt--;
			ppck->rBytesDone++;
		}

		// Dubs transfer
		while(ppck->store_dub_cnt) {
			if(!ppck->lim) return ePCK_OutBuffer;
			*(ppck->out) = ppck->store_dub_sym;
			ppck->out++; ppck->lim--;
			ppck->store_dub_cnt--;
			ppck->rBytesDone++;
		}

		if(ppck->rBytesDone == ppck->rBytes ) return ePCK_Done;

		// Dubs restore
		if(ppck->phase == ePCK_Phase_Data) {

			if(!ppck->len) return ePCK_InBuffer;
			ppck->store_dat_cnt = *(ppck->src); ppck->src++; ppck->len--; ppck->phase = ePCK_Phase_Dup;

		} else {

			if(ppck->len<2) return ePCK_InBuffer;
			ppck->store_dub_cnt = *(ppck->src); ppck->src++; ppck->len--;
			ppck->store_dub_sym = *(ppck->src); ppck->src++; ppck->len--;
			ppck->phase =  ePCK_Phase_Data;
		}

	}

	return ePCK_Error;
}

enPCKStatus VHPCK_UnpackFirst(stPCKSpacer *ppck, u8 *pSrc, int srclen, u8 *pResult, int buflim) {
	ppck->src = pSrc; ppck->len = srclen;
	ppck->out = pResult; ppck->lim = buflim;
	ppck->firstStart = 1; ppck->phase = ePCK_Phase_Data;
	ppck->store_dat_cnt = 0; ppck->store_dub_cnt = 0; ppck->rBytesDone = 0;
	return VHPCK_UnpackNext(ppck);
}

void VHPCK_Unpack_NewSrc(stPCKSpacer *ppck, u8 *ptr, int srclen) { ppck->src = ptr; ppck->len = srclen; }

void VHPCK_Unpack_NewDst(stPCKSpacer *ppck, u8 *result, int lim) { ppck->out = result; ppck->lim = lim; }
