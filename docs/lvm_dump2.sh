#!/bin/bash

function show_and_do()
{
	local cmd="$@"
	printf "\n%s\n\n" "$cmd"
	$cmd | sed 's/^  /	/'
	echo
}

cat <<EOF
vgs - volume group scan

	vg_name         - Name.
	pv_count        - Number of PVs.
	lv_count        - Number of LVs.
	vg_attr         - Various attributes - see man page.
	vg_size         - Total size of VG in current units.
	vg_free         - Total amount of free space in current units.
	vg_uuid         - Unique identifier.
	vg_extent_size  - Size of Physical Extents in current units.
	vg_extent_count - Total number of Physical Extents.
	vg_free_count   - Total number of unallocated Physical Extents.
	vg_seqno        - Revision number of internal metadata.  Incremented whenever it changes.
EOF

show_and_do "vgs --units=b --nosuffix --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno"

cat <<EOF
pvs - physical volume scan

	pv_name           - Name.
	vg_name           - Name.
	pv_attr           - Various attributes - see man page.
	pv_size           - Size of PV in current units.
	pv_free           - Total amount of unallocated space in current units.
	pv_uuid           - Unique identifier.
	dev_size          - Size of underlying device in current units.
	pe_start          - Offset to the start of data on the underlying device.
	pv_used           - Total amount of allocated space in current units.
	pv_pe_count       - Total number of Physical Extents.
	pv_pe_alloc_count - Total number of allocated Physical Extents.
	pvseg_start       - Physical Extent number of start of segment.
	pvseg_size        - Number of extents in segment.
EOF

show_and_do "pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count"
show_and_do "pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count,pvseg_start,pvseg_size"

cat <<EOF
lvs - logical volume scan

	lv_name              - Name.  LVs created for internal use are enclosed in brackets.
	vg_name              - Name.
	lv_attr              - Various attributes - see man page.
	lv_size              - Size of LV in current units.
	lv_uuid              - Unique identifier.
	lv_path              - Full pathname for LV.
	lv_kernel_major      - Currently assigned major number or -1 if LV is not active.
	lv_kernel_minor      - Currently assigned minor number or -1 if LV is not active.
	seg_count            - Number of segments in LV.
	vg_uuid              - Unique identifier.
	segtype              - Type of LV segment.
	stripes              - Number of stripes or mirror legs.
	stripe_size          - For stripes, amount of data placed on one device before switching to the next.
	seg_start            - Offset within the LV to the start of the segment in current units.
	seg_size             - Size of segment in current units.
	seg_pe_ranges        - Ranges of Physical Extents of underlying devices in command line format.
	devices              - Underlying devices used with starting extent numbers.
EOF

show_and_do "lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count"
show_and_do "lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,vg_uuid,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices"
show_and_do "lvs --all --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices"

