# Lithe
Lithe is a microkernel. Its design is similar to that of L4 family of kernels, currently it aims to run on modern hardware without support for legacy interfaces and chips. The primary architecture of development is x86_64 (there are plans for 32-bit x86 support), with considerations being taken to allowing porting over to RISC-V chips.

## Building and Running
You will need Clang and NASM to build the kernel and QEMU to run it. `make` will build the kernel image, and `make run` will start QEMU. You can also use `make run-debug` and then `make gdb` in another terminal to start debugging it in GDB.
