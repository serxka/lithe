CFLAGS := -std=gnu2x
CFLAGS += \
	-g \
	-O0 \
	-static
CFLAGS += \
	-Wall \
	-Wextra

CFLAGS += \
	-Isrc \
	-DDEBUG

ASFLAGS = -g

include build/config.mk
include build/${TOOLCHAIN}.mk
include build/${TARGET_ARCH}.mk

include src/kernel/build.mk
include src/utils/build.mk
include src/hw/build.mk

MKDIR = mkdir -p ${@D}

all: ${TARGETS}

run: ${KERNEL}
	qemu-system-${TARGET_ARCH} \
		-kernel $< \
		-no-shutdown \
		$(QEMU_ARGS)

run-debug: ${KERNEL}
	qemu-system-${TARGET_ARCH} \
		-kernel $< \
		-no-reboot \
		-no-shutdown \
		-s -S \
		$(QEMU_ARGS)

gdb:
	gdb --command=".gdb_connect"

clean:
	rm -rf ${OUTDIR}

C_SRC := $(shell find src/ -type f -name '*.h' -or -name '*.c')
fmt-dry:
	@clang-format --dry-run -i ${C_SRC}
fmt:
	@clang-format --verbose -i ${C_SRC}

.PHONY += all run run-debug gdb clean fmt-dry fmt
