// #include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include "vhpck21.hpp"

#define u8 uint8_t
#define u16 uint16_t


#define bufsize 1024
u8 packedData[bufsize]    = {0};

char * filename_packed      = (char*)"packed.bin";

u8 testVals[] = { 0x11, 0x12,0x12, 0x13,0x13,0x13, 0x14,0x14,0x14,0x14 };


#ifdef __linux__

bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    FILE *write_ptr = fopen(fname, "wb");
    fwrite(src, srclen, 1, write_ptr);
    return true;
}

#elif _WIN32

bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    WIN32H_CreateSingleFile(fname, src, srclen);
    return true;
}

#endif


void testpack() {
    using namespace VHPCK;

    u8 encodermem[Defs::memsize_unpck];
    VHPCK21 pck(encodermem);
    BitPtr wr(packedData);
    pck.EncodeChunk(&wr, testVals, sizeof(testVals) );

    int osize = 1024; // 1024 /*mPCK.PSize(packedData)
    WriteBinFile(filename_packed, packedData, osize);

    /*
	if(mPCK.CheckHDR(packedString)==VHPCK::vOk) {
        if(mPCK.Decode(packedString,restoredString,bufsize)==VHPCK::vOk) {
            WriteBinFile(filename_restored, (u8 *)restoredString, mPCK.OSize(packedString));
		}
	}
    */

}

#ifdef __linux__

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    testpack();
    return 0;
}

#elif __WIN32__

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    testpack();
    return 0;
}
#endif

// char * filename_restored    = (char*)"b.bin";
// char restoredString[bufsize] = {0};
