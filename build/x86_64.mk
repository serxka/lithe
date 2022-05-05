ARCH_CFLAGS := \
	-fno-stack-protector \
	-mcmodel=kernel \
	-mno-80387 \
	-mno-mmx \
	-mno-red-zone \
	-mno-sse \
	-mno-sse2

ARCH_ASFLAGS =

QEMU_ARGS := \
	-m 512M \
	-M q35 \
	-smp 2

CFLAGS += ${ARCH_CFLAGS}
ASFLAGS += ${ARCH_ASFLAGS}
