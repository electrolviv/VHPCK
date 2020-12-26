// #include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include "vhunpck21.hpp"
#include <sys/stat.h>
#include <stdlib.h>

#ifdef __linux__

#define bufsize 1024
u8 unpackedData[bufsize]    = {0};
u8 decodermem[VHPCK::Defs::memblks_unpck];

bool GetFileSize( char *fname, uint32_t * result ) {

    struct stat statbuf;

    if( stat( fname, &statbuf) == -1) {
        *result = 0;
      return false;
    }

    *result = (uint32_t)statbuf.st_size;

    return true;
}

bool ReadBinFile(char *fname, u8 *src, u16 srclen) {
    FILE *hread = fopen(fname, "rb");
    fread(src, srclen, 1, hread);
    fclose(hread);
    return true;
}

bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    FILE *write_ptr = fopen(fname, "wb");
    fwrite(src, srclen, 1, write_ptr);
    fclose(write_ptr);
    return true;
}

#elif _WIN32

bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    WIN32H_CreateSingleFile(fname, src, srclen);
    return true;
}

#endif


void testunpack(u8 * pmem, uint32_t srclen) {

    VHPCK::VHUNPCK21 unpck(decodermem);
    unpck.DecodeChunk(pmem, srclen, unpackedData);

}

#ifdef __linux__

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);

    if(argc == 2) {
        uint32_t fsize;
        if( GetFileSize(argv[1], &fsize)) {
            printf("Unpacking %s, %i bytes\n", argv[1], fsize);
            u8 * pmem = (u8 *)malloc(fsize);
            ReadBinFile(argv[1], pmem, fsize);
            testunpack(pmem, 10); // fsize
            free(pmem);
        }
    }
    return 0;
}

#elif __WIN32__

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    testpack();
    return 0;
}
#endif


// u8 encodermem[VHPCK::memblocksize];
// VHPCK::VHPCK21 pck(encodermem);
// VHPCK::BitPtr wr(packedData);
// pck.EncodeChunk(&wr, testVals, sizeof(testVals) );
// int osize = 1024; // 1024 /*mPCK.PSize(packedData)
// WriteBinFile(filename_packed, packedData, osize);

/*
if(mPCK.CheckHDR(packedString)==VHPCK::vOk) {
    if(mPCK.Decode(packedString,restoredString,bufsize)==VHPCK::vOk) {
        WriteBinFile(filename_restored, (u8 *)restoredString, mPCK.OSize(packedString));
    }
}
*/
