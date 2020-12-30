/*
 * (C) 2000-2020 V01G04A81
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


class DynMem {
    public:
        DynMem(uint32_t size=0) : ptr(nullptr), sz(0) { alloc(size); }
        ~DynMem() { dealloc(); }
        uint8_t * getptr() { return ptr; }
        uint32_t getsz() { return sz; }
        void alloc(uint32_t size) { dealloc();
            if(size) { ptr = (uint8_t *)malloc(size); sz = size; } }
        void dealloc() { if(sz) if(ptr) { free(ptr); ptr = nullptr; sz = 0; } }
    private:
        uint8_t *ptr; uint32_t sz; };
