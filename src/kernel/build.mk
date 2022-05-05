# The generated kernel file
KERNEL = ${OUTDIR}/kernel/kernel

# A list of all our source files
KERNEL_SRCS := \
	$(wildcard src/kernel/*.c) \
	$(wildcard src/kernel/arch/${TARGET_ARCH}/*.c) \
	$(wildcard src/kernel/arch/${TARGET_ARCH}/*.S) \
	$(wildcard src/utils/*/*.c) \
	$(wildcard src/utils/*.c) \
	$(wildcard src/hw/*/*.c) \
	$(wildcard src/hw/*.c)

# Pattern substitute them into objects out of tree
KERNEL_OBJS := $(patsubst src/%, ${OUTDIR}/%.o, ${KERNEL_SRCS})

# Special extra flags for building the kernel
K_CFLAGS := \
	${CFLAGS} \
	-ffreestanding \
	-nostdlib \
	-Isrc/kernel \
	-DLITHE_KERNEL

K_ASFLAGS := \
	${ASFLAGS} \
	-ffreestanding

# Rules to match only building files from the kernel build directory
${OUTDIR}/kernel/%.c.o: src/kernel/%.c
	@${MKDIR}
	${CC} ${K_CFLAGS} -c -MMD -o $@ $<
${OUTDIR}/kernel/%.S.o: src/kernel/%.S
	@${MKDIR}
	${AS} ${K_ASFLAGS} -c -MMD -o $@ $<

# Build our kernel image
${KERNEL}: ${KERNEL_OBJS}
	@${MKDIR}
ifeq ($(TARGET_ARCH),x86_64/)
	${LD} -T src/kernel/arch/${TARGET_ARCH}/link.ld --build-id=none -o $@.64 $^
	${OC} -I elf64-x86-64 -O elf32-i386 $@.64 $@
else
	${LD} -T src/kernel/arch/${TARGET_ARCH}/link.ld --build-id=none -o $@ $^
endif

# Add the kernel to our list of possible targets
TARGETS += ${KERNEL}
