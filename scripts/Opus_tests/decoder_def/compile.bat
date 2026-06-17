pause
gcc -o decoder.exe *.c   opus/src/*.c    opus/celt/*.c    opus/silk/*.c    opus/silk/fixed/*.c    -Iopus/include    -Iopus/celt    -Iopus/silk    -Iopus/silk/fixed   -DCUSTOM_MODES   -DOPUS_BUILD    -DFIXED_POINT    -DUSE_ALLOCA  -lm
