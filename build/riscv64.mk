ARCH_CFLAGS := \
	-fno-stack-protector \
	-mcmodel=medany \
	-mno-relax

ARCH_ASFLAGS = \
	-c \
	-ffreestanding \
	-mno-relax \
	-nostdlib

QEMU_ARGS := \
	-M virt \
	-cpu rv64 \
	-smp 4 \
	-m 512M \
	-device virtio-gpu-device \
	-device virtio-rng-device \
	-device virtio-net-device \
	-device virtio-tablet-device \
	-device virtio-keyboard-device

CFLAGS += ${ARCH_CFLAGS}
ASFLAGS += ${ARCH_ASFLAGS}
