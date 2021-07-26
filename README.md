# Posit-GCC (A Posit-enabled GCC compiler)
We implement Posit format in GCC. And we via compiler option fposit to switch IEEE 754 or Posit. You can reffer as below picture.
![](https://i.imgur.com/wL1xBhe.png)

Inorder to transform floating-point format, gcc via internal macro REAL_VALUE_TYPE to transform when parsing float senmatic statment. And in the assembly stage Posit format represent as decimal. The REAL_VALUE_TYPE lives in gcc/real.c and real.h. The architecture of compile float flow as below picture.  
![image](https://user-images.githubusercontent.com/51993200/125903994-78018b47-3b7b-44aa-bf10-73290237d3bf.png)

In the work, we use software emulation Posit compute. GCC provides a low-level runtime library, libgcc.a or libgcc_s.so.1 on some platforms. GCC generates calls to routines in this library automatically, whenever it needs to perform some operation that is too complicated to emit inline code for. About the function implement under gcc/libgcc/soft-fp folder.
### How to build
For x86-64, you can refer as following step to build GCC with Posit format in your machine.

    > git clone https://github.com/pengsheng-chen/Posit-GCC
    > cd Posit-GCC
    > mkdir build
    > cd build
    > ../configure --target=$TARGET --disable-nls --enable-languages=c,c++ --without-headers --prefix=/path/
    > make all-gcc
    > make all-target-libgcc
    > make all-target-libstdc++-v3
    > make install-gcc
    > make install-target-libgcc
    > make install-target-libstdc++-v3
    > make clean
Besides we need to build Posit library call libposit.a under gcc/libgcc/soft-fp folder. You can refer as following step that get the libposit.a library for Posit computing.

    > cd Posit-GCC/libgcc/soft-fp-2/
    > gcc -c -O2 -mno-sse -msoft-float -I../config/i386/ -I.. *.c
    > ar -crv libposit.a *.o
    > rm -f *.o
    
### Examples and Compile
After above build compiler and library that you can test the example code to validate the works. 

    > Posit-gcc -fposit -msoft-float example-code.c Posit-GCC/libgcc/soft-fp-2/libposit.a

```c=
void main(){
    float A = 1.323;
    float B = -0.792;
    float C = A + B;
    printf("Result A + B = %f\n", C); // Answer = 0.531
}
```
## Refference work 
[Cerlane Leong - SoftPosit](https://gitlab.com/cerlane/SoftPosit)  
[stillwater-sc - universal](https://github.com/stillwater-sc/universal)  
