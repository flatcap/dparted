/* the superblock at the front of the bitmap file -- little endian */
typedef struct bitmap_super_s {
	__le32 magic;        /*  0  BITMAP_MAGIC */
	__le32 version;      /*  4  the bitmap major for now, could change... */
	__u8  uuid[16];      /*  8  128 bit uuid - must match md device uuid */
	__le64 events;       /* 24  event counter for the bitmap (1)*/
	__le64 events_cleared;/*32  event counter when last bit cleared (2) */
	__le64 sync_size;    /* 40  the size of the md device's sync range(3) */
	__le32 state;        /* 48  bitmap state information */
	__le32 chunksize;    /* 52  the bitmap chunk size in bytes */
	__le32 daemon_sleep; /* 56  seconds between disk flushes */
	__le32 write_behind; /* 60  number of outstanding write-behind writes */
	__le32 sectors_reserved; /* 64 number of 512-byte sectors that are
				  * reserved for the bitmap. */

	__u8  pad[256 - 68]; /* set to zero */
} bitmap_super_t;

