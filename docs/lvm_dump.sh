#!/bin/bash

DIR="${0%/*}"

sudo vgs \
	--unquoted \
	--separator='	' \
	--units=b \
	--nosuffix \
	--nameprefixes \
	--noheadings \
	--options vg_uuid,vg_name,vg_seqno,pv_count,lv_count,vg_attr,vg_size,vg_free,vg_extent_size \
	| "$DIR/lvm.sed" | column -t > vgs

sudo lvs \
	--all \
	--unquoted \
	--separator='	' \
	--units=b \
	--nosuffix \
	--noheadings \
	--nameprefixes \
	--sort lv_kernel_minor \
	--options vg_uuid,lv_uuid,lv_name,lv_attr,mirror_log,lv_size,lv_path,lv_kernel_major,lv_kernel_minor,seg_count,segtype,stripes,stripe_size,seg_start_pe,devices \
	| "$DIR/lvm.sed" | column -t > lvs

sudo pvs \
	--unquoted \
	--separator='	' \
	--units=b \
	--nosuffix \
	--nameprefixes \
	--noheadings \
	--options pv_uuid,pv_size,pv_used,pv_attr,vg_extent_size,pvseg_start,pvseg_size,vg_name,vg_uuid,lv_uuid,lv_attr,segtype \
	| "$DIR/lvm.sed" | column -t > pvs

