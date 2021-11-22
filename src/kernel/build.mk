# The generated kernel file
KERNEL = ${OUTDIR}/kernel/kernel

# A list of all our source files
KERNEL_SRCS := \
	$(wildcard src/kernel/*.c) \
	$(wildcard src/kernel/arch/${TARGET_ARCH}/*.c) \
	$(wildcard src/kernel/arch/${TARGET_ARCH}/*.asm) \
	$(wildcard src/lithe/*/*.c) \
	$(wildcard src/lithe/*.c)
	
# Pattern substitute them into objects out of tree
KERNEL_OBJS := $(patsubst src/%, ${OUTDIR}/%.o, ${KERNEL_SRCS})

# Special extra flags for building the kernel
K_CFLAGS := \
	${CFLAGS} \
	-ffreestanding \
	-nostdlib \
	-Isrc/kernel

K_ASFLAGS := \
	${ASFLAGS}

# Rules to match only building files from the kernel build directory
${OUTDIR}/kernel/%.c.o: src/kernel/%.c
	@${MKDIR}
	${CC} ${K_CFLAGS} -c -MMD -o $@ $<
${OUTDIR}/kernel/%.asm.o: src/kernel/%.asm
	@${MKDIR}
	${AS} ${K_ASFLAGS} -MD -MF ${@:.o=.d} -o $@ $<

# Build our kernel image
${KERNEL}: ${KERNEL_OBJS}
	@${MKDIR}
	${CC} -T src/kernel/arch/${TARGET_ARCH}/link.ld ${K_CFLAGS} -Wl,--build-id=none -o $@.64 $^
	${OC} -I elf64-x86-64 -O elf32-i386 $@.64 $@

# Add the kernel to our list of possible targets
TARGETS += ${KERNEL}
