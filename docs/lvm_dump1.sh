echo "vgs --units=b --nosuffix --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno"
vgs --units=b --nosuffix --options vg_name,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_uuid,vg_extent_size,vg_extent_count,vg_free_count,vg_seqno
echo
echo "pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count"
pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count
echo
echo "pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count,pvseg_start,pvseg_size"
pvs --units=b --nosuffix --options pv_name,vg_name,pv_attr,pv_size,pv_free,pv_uuid,dev_size,pe_start,pv_used,pv_pe_count,pv_pe_alloc_count,pvseg_start,pvseg_size
echo
echo "lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count"
lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count
echo
echo "lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,vg_uuid,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices"
lvs --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_uuid,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,vg_uuid,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices
echo
echo "lvs --all --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices"
lvs --all --units=b --nosuffix --options lv_name,vg_name,lv_attr,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start,seg_size,seg_pe_ranges,devices
echo
