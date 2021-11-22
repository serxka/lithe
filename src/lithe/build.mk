# The generated kernel file
LIBLITHE = ${OUTDIR}/lithe/liblithe.a

# A list of all our source files
LITHE_SRCS := \
	$(wildcard src/lithe/*/*.c) \
	$(wildcard src/lithe/*.c)
	
# Pattern substitute them into objects out of tree
LITHE_OBJS := $(patsubst src/%, ${OUTDIR}/%.o, ${LITHE_SRCS})

# Rules to match only building files from the lithe build directory
${OUTDIR}/lithe/%.c.o: src/lithe/%.c
	@${MKDIR}
	${CC} ${CFLAGS} -c -MMD -o $@ $<
${OUTDIR}/lithe/%.asm.o: src/lithe/%.asm
	@${MKDIR}
	${AS} ${ASFLAGS} -MD -MF ${@:.o=.d} -o $@ $<

# Build our library archive
${LIBLITHE}: ${LITHE_OBJS}
	${AR} rcs $@ $<

# Add the archive to our list of possible targets
TARGETS += ${LIBLITHE}
