
#define VHPCK_USING_ENCODER 1
#include "lib_huffman.h"



u8 encodermem[1024];
char * filename_packed = (char*)"packed.bin";

const char strtst[] =
        "+--------------+"
        "|00000000000000|"
        "|00000000000000|"
        "|00000000000000|"
        "|00000000000000|"
        "|00000000000000|"
        "|00000000000000|"
        "+--------------+";

#define UNPACKED_BUFF_SIZE 1024
volatile char unpacked[UNPACKED_BUFF_SIZE];

VHPCK pck;

volatile verr rvale;
volatile verr rvald;

#ifdef __linux__
bool WriteBinFile(char *fname, void *src, uint16_t srclen) {
    FILE *write_ptr = fopen(fname, "wb");
    fwrite(src, srclen, 1, write_ptr);
    return true;
}
#endif

#if defined(_WIN32)
bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    WIN32H_CreateSingleFile(fname, src, srclen);
    return true;
}
#endif

void testpack() {

    rvale = pck.Encode(strtst, sizeof(strtst) - 1, encodermem, 1024);

#ifdef __linux__
    VHPCK::stHDR *phdr = (VHPCK::stHDR *)encodermem;
    WriteBinFile(filename_packed, encodermem, phdr->lenp);
    printf("PFX: x%.4X, OSize=%d bytes, PSize=%d bytes\n", phdr->pfx, phdr->leno, phdr->lenp);
#endif

}

void testunpack() {
    rvald = pck.Decode(encodermem, (void *)unpacked, UNPACKED_BUFF_SIZE);
#if defined(__linux__)
    VHPCK::stHDR *phdr = (VHPCK::stHDR *)encodermem;
    for(int i=0;i<phdr->leno;i++)
        printf("%c%s", unpacked[i], (i%16)==15 ? "\n":"");
#endif
}

#if defined(__linux__)

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    testpack();
    testunpack();
    return 0;
}

#elif defined(STM32)

int main() {
    testpack();
    testunpack();
    return 0;
}

#elif __WIN32__

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    testpack();
    return 0;
}
#endif

