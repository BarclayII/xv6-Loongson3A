/*
 * Based on Ocelot Linux port, which is
 * Copyright 2001 MontaVista Software Inc.
 * Author: jsun@mvista.com or jsun@junsun.net
 *
 * Copyright 2003 ICT CAS
 * Author: Michael Guo <guoyi@ict.ac.cn>
 *
 * Copyright (C) 2007 Lemote Inc. & Insititute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 *
 * Copyright (C) 2009 Lemote Inc.
 * Author: Wu Zhangjin, wuzhangjin@gmail.com
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_MACH_BOOT_PARAM_H_
#define __ASM_MACH_BOOT_PARAM_H_

#define DMA64_SUPPORTED 0x2

#define SYSTEM_RAM_LOW 1
#define SYSTEM_RAM_HIGH 2
#define MEM_RESERVED 3
#define PCI_IO 4
#define PCI_MEM 5
#define LOONGSON_CFG_REG 6
#define VIDEO_ROM 7
#define ADAPTER_ROM 8
#define ACPI_TABLE 9
#define SMBIOS_TABLE 10
#define MAX_MEMORY_TYPE 11

/* no use for pmon */
struct efi_systab {
	//efi_table_hdr_t hdr;	/* EFI header */
	unsigned long long fw_vendor;	/* physical addr of CHAR16 vendor string */
	unsigned long long fw_revision;	/* vesion of efi */
	unsigned long long con_in_handle;	/* control handle of input */
	unsigned long long con_in;	/* control input,support for inputing in kernel */
	unsigned long long con_out_handle;	/* control handle of input */
	unsigned long long con_out;	/* control input,support for inputing in kernel */
	unsigned long long stderr_handle;	/* standed error handle */
	//unsigned long stderr;	/* standed error */
	//efi_runtime_services_t runtime;	/* runtime service */
	unsigned long long boottime;	/* boottime */
	unsigned int nr_tables;	/* table id */
	unsigned long long tables;	/* all tables entry */
};

#define LOONGSON_BOOT_MEM_MAP_MAX 128
struct efi_memory_map_loongson {
	unsigned short vers;	/* version of efi_memory_map */
	unsigned int nr_map;	/* number of memory_maps */
	unsigned int mem_freq;	/* memory frequence */
	struct mem_map {
		unsigned int node_id;	/* node_id which memory attached to */
		unsigned int mem_type;	/* id of the node memory attached to */
		unsigned long long mem_start;	/* memory map start address */
		unsigned int mem_size;	/* for each memory_map size,not the total size */
	}map[LOONGSON_BOOT_MEM_MAP_MAX];
}__attribute__((packed));

enum loongson_cpu_type {
	Loongson_2F,Loongson_2E, Loongson_3A, Loongson_3B,Loongson_1A,Loongson_1B
};

enum board_type {
	RS780E,LS2H
};

struct efi_cpuinfo_loongson {
	/*
	 * Capability and feature descriptor structure for MIPS CPU
	 */
	unsigned short vers;	/* version of efi_cpuinfo_loongson */
	unsigned int processor_id;	/* processor id */
	enum loongson_cpu_type cputype;	/* 3a-3b */
	unsigned int total_node;	/* physical core number */
	unsigned int cpu_startup_core_id;	/* Core id */
	unsigned int cpu_clock_freq;	/* cpu_clock */
	unsigned int nr_cpus;
}__attribute__((packed));

struct system_loongson {
	unsigned short vers;	/* version of system_loongson */
	unsigned int ccnuma_smp;	/* 0:no numa; 1: has numa */
	unsigned int sing_double_channel;	/* 1:single; 2:double */
}__attribute__((packed));

struct irq_source_routing_table {
	unsigned short vers;
	unsigned short size;
	unsigned short rtr_bus, rtr_devfn;
	unsigned int vendor, device, PIC_type;	/* conform use HT or PCI to route to CPU-PIC */
	unsigned long long ht_int_bit;
	unsigned long long ht_enable;	/* irqs used in this PIC */
	unsigned int node_id;	/* node id */
	unsigned long long pci_mem_start_addr;
	unsigned long long pci_mem_end_addr;
	unsigned long long pci_io_start_addr;
	unsigned long long pci_io_end_addr;
	unsigned long long pci_config_addr;
}__attribute__((packed));

struct interface_info {
	unsigned short vers;	/* version of the specificition */
	unsigned short size;
	unsigned char flag;
	char description[64];
}__attribute__((packed));

#define MAX_RESOUCR_NUMBER 128
struct resource_loongson {
	unsigned long long start;	/* resource start address */
	unsigned long long end;		/* resource end address */
	char name[64];
	unsigned int flags;
};

struct archdev_data {};	/* arch specific additions */

struct board_devices {
	char name[64];	/* hold the device name */
	unsigned int num_resources;	/* number of device_resource */
	struct resource_loongson resource[MAX_RESOUCR_NUMBER];	/* for each device`s resource */
	struct archdev_data archdata;	/* arch specific additions */
};

struct loongson_special_attribute {
	unsigned short vers;	/* version of this special */
	char special_name[64];	/* special_atribute_name */
	unsigned int loongson_special_type;	/* tyoe of special device */
	struct resource_loongson resource[MAX_RESOUCR_NUMBER];	/* for each device`s resource */
};

struct loongson_params {
	unsigned long long memory_offset;	/* efi_memory_map_loongson struct offset */
	unsigned long long cpu_offset;		/* efi_cpuinfo_loongson struct offset */
	unsigned long long system_offset;	/* system_info struct offset */
	unsigned long long irq_offset;		/* irq_source_routing_table struct offset */
	unsigned long long interface_offset;	/* interface_info struct offset */
	unsigned long long special_offset;	/* loongson_special_attribute struct offset */
	unsigned long long boarddev_table_offset;	/* board_device offset */
};

struct smbios_tables {
	u16 vers;	/* version of smbios */
	unsigned long long vga_bios;	/* vga_bios address */
	struct loongson_params lp;
};

struct efi_reset_system_t {
	unsigned long long ResetCold;
	unsigned long long ResetWarm;
	unsigned long long ResetType;
	unsigned long long Shutdown;
};

struct efi_loongson {
	//efi_system_table_t systab;	/* EFI system table */
	unsigned long long mps;	/* MPS table */
	unsigned long long acpi;	/* ACPI table (IA64 ext 0.71) */
	unsigned long long acpi20;	/* ACPI table (ACPI 2.0) */
	struct smbios_tables smbios;	/* SM BIOS table */
	unsigned long long sal_systab;	/* SAL system table */
	unsigned long long boot_info;	/* boot info table */
	/*
	  efi_get_time_t *get_time;
	  efi_set_time_t *set_time;
	  efi_get_wakeup_time_t *get_wakeup_time;
	  efi_set_wakeup_time_t *set_wakeup_time;
	  efi_get_variable_t *get_variable;
	  efi_get_next_variable_t *get_next_variable;
	  efi_set_variable_t *set_variable;
	  efi_get_next_high_mono_count_t *get_next_high_mono_count;
	  efi_reset_system_t *reset_system;
	  efi_set_virtual_address_map_t *set_virtual_address_map;
	*/
};

struct boot_params {
	//struct screen_info *screen_info;
	//struct sys_desc_table *sys_desc_table;
	struct efi_loongson efi;
	struct efi_reset_system_t reset_system;
};

extern unsigned long pci_mem_start_addr, pci_mem_end_addr;
extern unsigned long long ht_control_base;
extern unsigned long long loongson_int_bit_ht_int0;
extern unsigned long long io_base_regs_addr;
extern unsigned long long smp_group1;
extern unsigned long long smp_group0;
extern unsigned long long smp_group2;
extern unsigned long long smp_group3;
extern unsigned long long loongson_pciio_base;
extern unsigned long long loongson_htio_base;
extern unsigned long long rtc_always_bcd;
extern unsigned int loongson3;
extern unsigned long ht_enable;
extern unsigned long ht_int_bit;
extern unsigned int sing_double;
extern unsigned int ccnuma;
extern unsigned int nr_cpu_loongson;
extern enum loongson_cpu_type cputype;
extern struct efi_memory_map_loongson *emap;
extern enum board_type board_type;
extern unsigned int nodes_loongson;
extern unsigned int CONFIG_NODES_SHIFT_LOONGSON;
extern uint32_t dma64_supported;

extern unsigned int Loongson3B05_uncache;

#define CONFIG_DMA_NONCOHERENT Loongson3B05_uncache

#endif
