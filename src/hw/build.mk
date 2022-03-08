# Rules to match only building files from the hw build directory
${OUTDIR}/hw/%.c.o: src/hw/%.c
	@${MKDIR}
	${CC} ${CFLAGS} -c -MMD -o $@ $<
${OUTDIR}/hw/%.asm.o: src/hw/%.asm
	@${MKDIR}
	${AS} ${ASFLAGS} -MD -MF ${@:.o=.d} -o $@ $<
