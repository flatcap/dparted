#ifndef _LVM2_H_
#define _LVM2_H_

#include <cstdint>

/* Fields with the suffix _xl should be xlate'd wherever they appear */

#define FMTT_MAGIC "\040\114\126\115\062\040\170\133\065\101\045\162\060\116\052\076"
#define FMTT_VERSION 1

struct label_header {
	int8_t id[8];		/* LABELONE */
	uint64_t sector_xl;	/* Sector number of this label */
	uint32_t crc_xl;	/* From next field to end of sector */
	uint32_t offset_xl;	/* Offset from start of struct to contents */
	int8_t type[8];		/* LVM2 001 */
} __attribute__((__packed__));

struct disk_locn {
	uint64_t offset;	/* Offset in bytes to start sector */
	uint64_t size;		/* Bytes */
} __attribute__((__packed__));

struct pv_header {
	int8_t pv_uuid[32];

	/* This size can be overridden if PV belongs to a VG */
	uint64_t device_size_xl;	/* Bytes */

	/* NULL-terminated list of data areas followed by */
	/* NULL-terminated list of metadata area headers */
	struct disk_locn disk_areas_xl[8];	/* Two lists */
} __attribute__((__packed__));

struct raw_locn {
	uint64_t offset;	/* Offset in bytes to start sector */
	uint64_t size;		/* Bytes */
	uint32_t checksum;
	uint32_t flags;
} __attribute__((__packed__));

/* Structure size limited to one sector */
struct mda_header {
	uint32_t checksum_xl;	/* Checksum of rest of mda_header */
	int8_t magic[16];	/* FMTT_MAGIC To aid scans for metadata */
	uint32_t version;
	uint64_t start;		/* Absolute start byte of mda_header */
	uint64_t size;		/* Size of metadata area */

	struct raw_locn raw_locns[4];	/* NULL-terminated list */
} __attribute__((__packed__));

#endif // _LVM2_H_

