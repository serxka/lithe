# The generated binary file
UTILS = ${OUTDIR}/utils/utils.a

# A list of all our source files
UTILS_SRCS := \
	$(wildcard src/utils/*/*.c) \
	$(wildcard src/utils/*.c)
	
# Pattern substitute them into objects out of tree
UTILS_OBJS := $(patsubst src/%, ${OUTDIR}/%.o, ${UTILS_SRCS})

# Rules to match only building files from the utils build directory
${OUTDIR}/utils/%.c.o: src/utils/%.c
	@${MKDIR}
	${CC} ${CFLAGS} -c -MMD -o $@ $<
${OUTDIR}/utils/%.asm.o: src/utils/%.asm
	@${MKDIR}
	${AS} ${ASFLAGS} -MD -MF ${@:.o=.d} -o $@ $<

# Build our library archive
${UTILS}: ${UTILS_OBJS}
	${AR} rcs $@ $<

# Add the archive to our list of possible targets
TARGETS += ${UTILS}
