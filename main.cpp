#include <windows.h>
#include "lib_huffman.h"

//---------------------------------------------------------------------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	#define bufsize 1024
	char testString[] = {
			"This is a test string ! Packing & unpacking with classic Huffman encoding ..."\
			"This is a test string ! Packing & unpacking with classic Huffman encoding ..."\
			"This is a test string ! Packing & unpacking with classic Huffman encoding ..."\
			"This is a test string ! Packing & unpacking with classic Huffman encoding ..." };
	char packedString[bufsize];
	char restoredString[bufsize];

	VHPCK mPCK;
	mPCK.Encode(testString, (u16)strlen(testString),(u8 *)packedString,bufsize);
	WIN32H_CreateSingleFile("a.bin", packedString, mPCK.PSize(packedString));

	if(mPCK.CheckHDR(packedString)==VHPCK::vOk) {
		if(mPCK.Decode(packedString,restoredString,bufsize)==VHPCK::vOk) { // Done !
			WIN32H_CreateSingleFile("b.bin", restoredString, mPCK.OSize(packedString));
		}
	}

	return 0;
}