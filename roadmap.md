# Roadmap

## Milestone 1 - Heading userspace
Basic tasks needed to get us into userspace.
- [x] Basic Multiboot x86\_64 stub
- [x] GDT/IDT
- [x] Physical Page Allocator
- [x] Virtual Page Allocator
- [ ] Parse MADT
- [ ] Configure APIC
- [ ] HPET
- [ ] SMP
- [ ] Scheduling
- [x] Syscalls
- [ ] SIMD (sse, fpu, avx, etc.)
	- Lazily save and store values for this when doing system calls. Allows faster context switching in most cases where they aren't used. When they are used #GP will be caused allowing us to save the context and switch it over fully.

## Milestone 2 - Syscalls, Application IPC, Userspace
Make userspace usable, after this is done the kernel should be *technically* complete. Server implementations follow.
- [ ] syscalls
	- [ ] send
	- [ ] recv
	- [ ] reply
	- [ ] call
	- [ ] yield
	- [ ] nbrecv
- [ ] Application IPC
- [ ] Basic root server

## Milestone 3 - Profit ? ? ?
- some drivers and servers to go along with them

## Target Architectures
- [ ] x86_64
- [ ] riscv
- [ ] arm64
