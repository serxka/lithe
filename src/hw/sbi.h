#pragma once

#include <stdint.h>

struct sbiret {
	long error;
	long value;
};

typedef enum {
	SBI_SUCCESS = 0,
	SBI_ERR_FAILED = -1,
	SBI_ERR_NOT_SUPPORTED = -2,
	SBI_ERR_INVALID_PARAM = -3,
	SBI_ERR_DENIED = -4,
	SBI_ERR_INVALID_ADDRESS = -5,
	SBI_ERR_ALREADY_AVALIABLE = -6,
	SBI_ERR_ALREADY_STARTED = -7,
	SBI_ERR_ALREADY_STOPPED = -8
} sbi_errors_t;

typedef unsigned long sbiword_t;

typedef enum {
	SBI_LEGACY_EID_SET_TIMER = 0x00,
	SBI_LEGACY_EID_CONSOLE_PUTCHAR = 0x01,
	SBI_LEGACY_EID_CONSOLE_GETCHAR = 0x02,
	SBI_LEGACY_EID_CLEAR_IPI = 0x03,
	SBI_LEGACY_EID_SEND_IPI = 0x04,
	SBI_LEGACY_EID_REMOTE_FENCEI = 0x05,
	SBI_LEGACY_EID_REMOTE_SFENCEVMA = 0x06,
	SBI_LEGACY_EID_REMOTE_SFENCEVMA_ASID = 0x07,
	SBI_LEGACY_EID_SHUTDOWN = 0x08,
	SBI_EID_BASE = 0x10,
	SBI_EID_TIME = 0x54494D45,
	SBI_EID_IPI = 0x735049,
	SBI_EID_RFENCE = 0x52464E43,
	SBI_EID_HSM = 0x48534D,
	SBI_EID_SYSTEM_RESET = 0x53525354,
} sbi_ext_t;

typedef enum {
	SBI_FID_LEGACY = 0x0,
	SBI_FID_SPEC_VERSION = 0x0,
	SBI_FID_IMPL_ID = 0x1,
	SBI_FID_IMPL_VERSION = 0x2,
	SBI_FID_PROBE_EXT = 0x3,
	SBI_FID_VENDOR_ID = 0x4,
	SBI_FID_MARCH_ID = 0x5,
	SBI_FID_MIMPL_ID = 0x6,
	SBI_FID_SET_TIMER = 0x0,
	SBI_FID_SEND_IPI = 0x0,
	SBI_FID_FENCE_I = 0x0,
	SBI_FID_SFENCE_VMA = 0x1,
	SBI_FID_SFENCE_VMA_ASID = 0x2,
	SBI_FID_HFENCE_GVMA_VMID = 0x3,
	SBI_FID_HFENCE_GVMA = 0x4,
	SBI_FID_HFENCE_VVMA_ASID = 0x5,
	SBI_FID_HFENCE_VVMA = 0x6,
	SBI_FID_HART_START = 0x0,
	SBI_FID_HART_STOP = 0x1,
	SBI_FID_HART_GET_STATUS = 0x2,
	SBI_FID_HART_SUSPEND = 0x3,
	SBI_FID_SYSTEM_RESET = 0x0
} sbi_func_t;

typedef enum {
	SBI_IMPL_BBL = 0x0,
	SBI_IMPL_OPENSBI = 0x1,
	SBI_IMPL_XVISOR = 0x2,
	SBI_IMPL_KVM = 0x3,
	SBI_IMPL_RUSTSBI = 0x4,
	SBI_IMPL_DIOSIX = 0x5,
	SBI_IMPL_COFFER = 0x6,
} sbi_impl_id_t;

typedef enum {
	SBI_HART_STARTED = 0x0,
	SBI_HART_STOPPED = 0x1,
	SBI_HART_START_PENDING = 0x2,
	SBI_HART_STOP_PENDING = 0x3,
	SBI_HART_SUSPENDED = 0x4,
	SBI_HART_SUSPEND_PENDING = 0x5,
	SBI_HART_RESUME_PENDING = 0x6,
} sbi_hart_states_t;

typedef enum {
	SBI_SYSTEM_SHUTDOWN = 0x0,
	SBI_SYSTEM_COLD_REBOOT = 0x1,
	SBI_SYSTEM_WARM_REBOOT = 0x2,
} sbi_system_reset_types_t;

typedef enum {
	SBI_REASON_NONE = 0x0,
	SBI_REASON_FAILURE = 0x1,
} sbi_system_reset_reason_t;

static inline struct sbiret _sbi_call0(sbi_ext_t ext, sbiword_t func) {
	register sbiword_t r_ext asm("a7") = ext;
	register sbiword_t r_func asm("a6") = func;
	register sbiword_t r_a0 asm("a0");
	register sbiword_t r_a1 asm("a1");
	__asm__ __volatile__("ecall"
	                     : "=r"(r_a0), "=r"(r_a1)
	                     : "r"(r_ext), "r"(r_func));
	return (struct sbiret){r_a0, r_a1};
}

static inline struct sbiret _sbi_call1(sbi_ext_t ext, sbiword_t func,
                                       sbiword_t arg0) {
	register sbiword_t r_ext asm("a7") = ext;
	register sbiword_t r_func asm("a6") = func;
	register sbiword_t r_a0 asm("a0") = arg0;
	register sbiword_t r_a1 asm("a1");
	__asm__ __volatile__("ecall"
	                     : "+r"(r_a0), "=r"(r_a1)
	                     : "r"(r_ext), "r"(r_func));
	return (struct sbiret){r_a0, r_a1};
}

static inline struct sbiret _sbi_call2(sbi_ext_t ext, sbiword_t func,
                                       sbiword_t arg0, sbiword_t arg1) {
	register sbiword_t r_ext asm("a7") = ext;
	register sbiword_t r_func asm("a6") = func;
	register sbiword_t r_a0 asm("a0") = arg0;
	register sbiword_t r_a1 asm("a1") = arg1;
	__asm__ __volatile__("ecall"
	                     : "+r"(r_a0), "=r"(r_a1)
	                     : "r"(r_ext), "r"(r_func));
	return (struct sbiret){r_a0, r_a1};
}

static long sbi_console_putchar(int ch) {
	register sbiword_t r_ext asm("a7") = SBI_LEGACY_EID_CONSOLE_PUTCHAR;
	register sbiword_t r_a0 asm("a0") = ch;
	__asm__ __volatile__("ecall" : "+r"(r_a0) : "r"(r_ext));
	return r_a0;
}

static long sbi_console_getchar(void) {
	register sbiword_t r_ext asm("a7") = SBI_LEGACY_EID_CONSOLE_GETCHAR;
	register sbiword_t r_a0 asm("a0");
	__asm__ __volatile__("ecall" : "=r"(r_a0) : "r"(r_ext));
	return r_a0;
}

static long sbi_clear_ipi(void) {
	register sbiword_t r_ext asm("a7") = SBI_LEGACY_EID_CLEAR_IPI;
	register sbiword_t r_a0 asm("a0");
	__asm__ __volatile__("ecall" : "=r"(r_a0) : "r"(r_ext));
	return r_a0;
}

static inline struct sbiret sbi_get_spec_version(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_SPEC_VERSION);
}

static inline struct sbiret sbi_get_impl_id(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_IMPL_ID);
}

static inline struct sbiret sbi_get_impl_version(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_IMPL_VERSION);
}

static inline struct sbiret sbi_probe_extension(sbiword_t extension_id) {
	return _sbi_call1(SBI_EID_BASE, SBI_FID_PROBE_EXT, extension_id);
}

static inline struct sbiret sbi_get_mvendor_id(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_VENDOR_ID);
}

static inline struct sbiret sbi_get_marchid(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_MARCH_ID);
}

static inline struct sbiret sbi_get_mimpid(void) {
	return _sbi_call0(SBI_EID_BASE, SBI_FID_MIMPL_ID);
}

static inline struct sbiret sbi_set_timer(sbiword_t stime_value) {
	return _sbi_call1(SBI_EID_TIME, SBI_FID_SET_TIMER, stime_value);
}

static inline struct sbiret sbi_send_ipi(sbiword_t hart_mask,
                                         sbiword_t hart_mask_base) {
	return _sbi_call2(SBI_EID_IPI, SBI_FID_SEND_IPI, hart_mask,
	                  hart_mask_base);
}

static inline struct sbiret sbi_system_reset(sbi_system_reset_types_t type,
                                             sbi_system_reset_reason_t reason) {
	return _sbi_call2(SBI_EID_SYSTEM_RESET, SBI_FID_SYSTEM_RESET, type,
	                  reason);
}
