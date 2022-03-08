#pragma once

#include <hw/acpi/tables.h>
#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

#define ACPI_MADT_SIG str_n$(4, "APIC")

typedef struct {
	uint8_t type;
	uint8_t len;
} PACKED madt_record_t;

typedef struct {
	acpi_stdh_t header;
	uint32_t lapic;
	uint32_t flags;
	madt_record_t records[];
} PACKED acpi_madt_t;

enum {
	ACPI_MATD_RECORD_LAPIC = 0,
	ACPI_MATD_RECORD_IOAPIC = 1,
	ACPI_MATD_RECORD_ISO = 2, // Interrupt Source Override
	ACPI_MATD_RECORD_IO_NMI = 3,
	ACPI_MATD_RECORD_L_NMI = 4,
	ACPI_MATD_RECORD_LAPIC_OVERRIDE = 5,
};

typedef struct {
	madt_record_t record;
	uint8_t processor_id;
	uint8_t id;
	uint32_t flags;
} PACKED madt_lapic_record_t;

typedef struct {
	madt_record_t record;
	uint8_t id;
	uint8_t _reserved;
	uint32_t address;
	uint32_t interrupt_base;
} PACKED madt_ioapic_record_t;

typedef struct {
	madt_record_t record;
	uint8_t bus;
	uint8_t irq;
	uint32_t gsi;
	uint16_t flags;
} PACKED madt_iso_record_t;

typedef struct {
	madt_record_t record;
	uint8_t source;
	uint8_t _reserved;
	uint16_t flags;
	uint32_t gsi;
} PACKED madt_io_nmi_record_t;

typedef struct {
	madt_record_t record;
	uint16_t _reserved;
	uint64_t address;
} PACKED madt_lpaic_override_record_t;
