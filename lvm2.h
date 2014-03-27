#ifndef _LVM2_H_
#define _LVM2_H_

//XXX copyright? ownership?

#include <cstdint>

// " LVM2 x[5A%r0N*>"
#define FMTT_MAGIC "\040\114\126\115\062\040\170\133\065\101\045\162\060\116\052\076"
#define FMTT_VERSION 1

struct label_header {
	int8_t id[8];		/* LABELONE */
	uint64_t sector;	/* Sector number of this label */
	uint32_t crc;		/* From next field to end of sector */
	uint32_t offset;	/* Offset from start of struct to contents */
	int8_t type[8];		/* LVM2 001 */
} __attribute__((__packed__));

struct disk_locn {
	uint64_t offset;	/* Offset in bytes to start sector */
	uint64_t size;		/* Bytes */
} __attribute__((__packed__));

struct pv_header {
	int8_t pv_uuid[32];

	/* This size can be overridden if PV belongs to a VG */
	uint64_t device_size;	/* Bytes */

	/* nullptr-terminated list of data areas followed by */
	/* nullptr-terminated list of metadata area headers */
	struct disk_locn disk_areas[8];	/* Two lists */
} __attribute__((__packed__));

struct raw_locn {
	uint64_t offset;	/* Offset in bytes to start sector */
	uint64_t size;		/* Bytes */
	uint32_t checksum;
	uint32_t flags;
} __attribute__((__packed__));

/* Structure size limited to one sector */
struct mda_header {
	uint32_t checksum;	/* Checksum of rest of mda_header */
	int8_t magic[16];	/* FMTT_MAGIC To aid scans for metadata */
	uint32_t version;
	uint64_t start;		/* Absolute start byte of mda_header */
	uint64_t size;		/* Size of metadata area */

	struct raw_locn raw_locns[4];	/* nullptr-terminated list */
} __attribute__((__packed__));

#endif // _LVM2_H_

