#ifndef VHPCK21DEFS_HPP
#define VHPCK21DEFS_HPP

#include "vhplatform.h"
#include "bitptr.hpp"

// 'VHH2' 'CRC16' 'ATTRS32' 'SIZE32ORIG' 'SIZE32PCK'

// 00 - END
// 01 - Packed
// 10 - Unpacked
// 11 - Spacer


namespace VHPCK {

class Defs {
public:
    static const u8  inv = 255;
    static const u8  elmslast = 32 - 1;
    static const int memblks_pck = 7;
    static const int memblks_unpck = 3;
    static const int memsize_pck = memblks_pck * 256;
    static const int memsize_unpck = memblks_unpck * 256;
};

}



#endif // VHPCK21DEFS_HPP
