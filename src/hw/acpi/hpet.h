#pragma once

#include <hw/acpi/tables.h>
#include <utils/base/attributes.h>
#include <utils/base/defs.h>
#include <utils/text/str.h>

#define ACPI_HPET_SIG str_n$(4, "HPET")

typedef struct {
	uint8_t address_mode;
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t reserved;
	uint64_t address;
} PACKED acpi_address_t;

typedef struct {
	acpi_stdh_t header;
	uint8_t hardware_rev_id;
	uint8_t comparator_count : 5;
	uint8_t counter_size : 1;
	uint8_t reserved : 1;
	uint8_t legacy_replacement : 1;
	uint16_t pci_vendor_id;
	acpi_address_t address;
	uint8_t hpet_tick;
	uint16_t minimum_tick;
	uint8_t page_protection;
} PACKED acpi_hpet_t;