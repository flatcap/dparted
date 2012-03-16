function populate_ext4()
{
	local dev="$1"
	local size="$2"
	local dir="$(mktemp -d)"

	mount $dev $dir
	fallocate --length $size $dir/filler
	umount $dir
	rmdir $dir
}

function populate()
{
	local dev="$1"
	local size="$2"
	local dir="$(mktemp -d)"

	mount $dev $dir
	dd if=/dev/zero bs=1M count=$size of=$dir/filler
	umount $dir
	rmdir $dir
}


function gpt_init()
{
	[ -z "$1" ] && return
	[ -z "$2" ] || return
	[ -f "$1" ] || return

	cat <<-EOF | parted $1
		mklabel gpt
		quit
	EOF
}

function gpt_print()
{
	[ -z "$1" ] && return
	[ -z "$2" ] || return
	[ -f "$1" ] || return

	cat <<-EOF | parted $1
		unit s
		print free
		quit
	EOF

}

function gpt_create()
{
	[ -z "$7" ] && return
	[ -z "$8" ] || return
	[ -b "$1" ] || return
	[ -f "$2" ] || return

	local LOOP="$1"
	local FILE="$2"
	local PLABEL="$3"
	local START="$4"
	local SIZE="$5"
	local FLABEL="$6"
	local FILL="$7"
	local END="$((START+SIZE))"

	START=$((START*2048))
	SIZE=$((SIZE*2048))
	END=$((END*2048))

	cat <<-EOF | parted $FILE
		unit s
		mkpart primary $START $((END-1))
		name $COUNT $PLABEL
		quit
	EOF

	START=$((START*512))
	SIZE=$((SIZE*512))

	losetup $LOOP $FILE --offset $START --sizelimit $SIZE
	mke2fs -t ext4 -L $FLABEL $LOOP
	populate_ext4 $LOOP ${FILL}M
	losetup -d $LOOP

	: $((COUNT++))
}


