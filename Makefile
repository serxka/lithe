CFLAGS := -std=gnu2x
CFLAGS += \
	-g \
	-O0 \
	-static
CFLAGS += \
	-Wall \
	-Wextra

CFLAGS += \
	-Isrc

ASFLAGS = 

include build/config.mk
include build/${TOOLCHAIN}.mk
include build/${TARGET_ARCH}.mk

include src/kernel/build.mk
include src/lithe/build.mk

MKDIR = mkdir -p ${@D}

all: ${ALL}

run: ${KERNEL}
	qemu-system-${TARGET_ARCH} \
		-kernel $< \
		-no-shutdown \
		$(QEMU_ARGS)

clean:
	rm -rf ${OUTDIR}

C_SRC := $(shell find src/ -type f -name '*.h' -or -name '*.c')
dry-format:
	@clang-format --dry-run -i ${C_SRC}
format:
	@clang-format --verbose -i ${C_SRC}

.PHONY = all clean run dry-format format
