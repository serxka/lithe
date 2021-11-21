SRC = kernel
INCLUDE = include

ARCH = x86_64

# Programs
AS = nasm
CC = clang --target=${ARCH}-none-elf
LD = ld
OC = llvm-objcopy

# C Compiler Flags
CFLAGS = -ffreestanding -static -std=gnu11 -nostdlib -O0 -g
# Arch specific arguments
CFLAGS += -mno-red-zone -fno-omit-frame-pointer -fno-stack-protector -mno-sse -mno-80387 -mno-mmx -mcmodel=kernel
# Warnings
CFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -pedantic

# C-preprocessor flags
CPPFLAGS = -D_KERNEL -D_KERNEL_ARCH=${ARCH} -I${SRC} -I${INCLUDE} -I.

# Assembler flags
ASFLAGS = -felf64

C_OBJS = $(patsubst %.c,%.o,$(wildcard ${SRC}/*.c))
C_OBJS += $(patsubst %.c,%.o,$(wildcard ${SRC}/*/*.c))
C_OBJS += $(patsubst %.c,%.o,$(wildcard ${SRC}/arch/${ARCH}/*.c))

AS_OBJS = $(patsubst %.asm,%.o,$(wildcard ${SRC}/arch/${ARCH}/*.asm))

all: kernel.32

run: kernel.32
	qemu-system-x86_64 -kernel $< -smp 1 -m 512m

kernel.64: ${C_OBJS} ${AS_OBJS}
	${CC} -T ${SRC}/arch/${ARCH}/link.ld ${CFLAGS} -Wl,--build-id=none -o $@ $^

kernel.32: kernel.64
	${OC} -I elf64-x86-64 -O elf32-i386 $< $@

%.o: %.c
	${CC} ${CFLAGS} ${CPPFLAGS} -c -MMD -o $@ $<

%.o: %.asm
	${AS} $(ASFLAGS) -MD -MF ${@:.o=.d} -o $@ $<

C_SRCS := $(shell find . -type f -name '*.h' -or -name '*.c')

dry-format:
	@clang-format --dry-run -i ${C_SRCS}

format:
	@clang-format --verbose -i ${C_SRCS}

clean:
	rm -f ${C_OBJS} ${C_OBJS:.o=.d}
	rm -f ${AS_OBJS} ${AS_OBJS:.o=.d}
	rm -f kernel.32 kernel.64

.PHONY = all clean run dry-format format
