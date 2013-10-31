#define DM_RAID_MAGIC 0x64526D44
struct dm_raid_superblock {
	__le32 magic;		/* "DmRd" */
	__le32 features;	/* Used to indicate possible future changes */

	__le32 num_devices;	/* Number of devices in this array. (Max 64) */
	__le32 array_position;	/* The position of this drive in the array */

	__le64 events;		/* Incremented by md when superblock updated */
	__le64 failed_devices;	/* Bit field of devices to indicate failures */

	/*
	 * This offset tracks the progress of the repair or replacement of
	 * an individual drive.
	 */
	__le64 disk_recovery_offset;

	/*
	 * This offset tracks the progress of the initial array
	 * synchronisation/parity calculation.
	 */
	__le64 array_resync_offset;

	/*
	 * RAID characteristics
	 */
	__le32 level;
	__le32 layout;
	__le32 stripe_sectors;

	__u8 pad[452];		/* Round struct to 512 bytes. */
				/* Always set to 0 when writing. */
} __packed;

