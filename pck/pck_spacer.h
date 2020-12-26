/*
 * pck_spacer.h
 *
 *  Created on: Feb 24, 2016
 *      Author: Viktor
 */

#ifndef PCK_SPACER_H_
#define PCK_SPACER_H_

#include "vhplatform.h"

typedef enum {
	ePCK_Done = 0,
	ePCK_InBuffer,
	ePCK_OutBuffer,
	ePCK_Error
} enPCKStatus;

typedef enum {
	ePCK_OpType_Pack = 0,
	ePCK_OpType_Unpack
} enPCKType;

typedef enum {
	ePCK_Phase_Data = 0,
	ePCK_Phase_Dup
} enPCKPhase;

typedef enum {
	ePCK_BUFF_Cached = 0,
	ePCK_BUFF_Full,
	ePCK_BUFF_Pattern
} enPCKBuffState;

typedef struct {

	u8 *src; u32 len;
	u8 *out; u32 lim;

	enPCKPhase phase;

	// enPCKType optype;
	/* u8 buff[256]; u8 buffcnt; u8 buffsave; */
	// u8 oldsym;
	// u8 oldsym_cnt;
	// u8 finalization;
	int firstStart;
	int rBytes;
	int rBytesDone;

	u8 store_dat_cnt;
	u8 store_dub_cnt;
	u8 store_dub_sym;

} stPCKSpacer;

int Pack(u8 *src_dat, int src_len, u8 *result, int res_len);

enPCKStatus VHPCK_UnpackFirst(stPCKSpacer *ppck, u8 *pSrc, int srclen, u8 *pResult, int buflim);
enPCKStatus VHPCK_UnpackNext(stPCKSpacer *ppck);

void VHPCK_Unpack_NewSrc(stPCKSpacer *ppck, u8 *ptr, int srclen);
void VHPCK_Unpack_NewDst(stPCKSpacer *ppck, u8 *result, int lim);

#endif /* PCK_SPACER_H_ */
