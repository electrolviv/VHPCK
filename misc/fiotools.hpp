/*
 * (C) 2000-2020 V01G04A81
 */

#ifndef FIOTOOLS_HPP
#define FIOTOOLS_HPP

// #include "vhplatform.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

namespace VHPCK {

class VHFIOTools {

public:

#ifdef __linux__

    static bool ReadBinFile(char *fname, uint8_t *src, uint32_t srclen) {
        FILE *hread = fopen(fname, "rb");
        fread(src, srclen, 1, hread);
        fclose(hread);
        return true;
    }

    static bool WriteBinFile(char *fname, uint8_t *src, uint32_t srclen) {
        FILE *write_ptr = fopen(fname, "wb");
        fwrite(src, srclen, 1, write_ptr);
        fclose(write_ptr);
        return true;
    }

    static bool GetFileSize( char *fname, uint32_t * result ) {
        struct stat statbuf;
        if( stat( fname, &statbuf) == -1) {
            *result = 0;
          return false;
        }
        *result = (uint32_t)statbuf.st_size;
        return true;
    }


#elif _WIN32

    static bool WriteBinFile(char *fname, u8 *src, u16 srclen) {
    WIN32H_CreateSingleFile(fname, src, srclen);
    return true;
}

#endif

};

}

#endif // FIOTOOLS_HPP
