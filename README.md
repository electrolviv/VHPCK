### VHPCK

Data compression / decompression example for embedded application

pck10 - First version, Rev 1.0
pck21 - Second version, Rev 2.1

(C) V01G04A81

*** Build testpck10 with CMake

```
$>mkdir build
$>cd build
$>cmake ..
$>make
```


*** Build testpck10 on STM32 platform
```
#!/bin/sh
arm-none-eabi-g++ -O0 -DSTM32 -mcpu=cortex-m4 -I../../../pck10 -c testpck10.cpp
arm-none-eabi-objdump -Sg testpck10.o > testpck10.s
```

Output:
```
$ ./testpackv10
PFX: x31484856, OSize=129 bytes, PSize=43 bytes
+--------------+
|00000000000000|
|00000000000000|
|00000000000000|
|00000000000000|
|00000000000000|
|00000000000000|
+--------------+
```
