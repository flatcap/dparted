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


##
# Print OK in green
function ok()
{
	echo -e "\e[01;32mOK\e[0m $@"
}

##
# Print Error in red
function error()
{
	echo -e "\e[01;31mError\e[0m"
	false
}


##
# remove loops and images from previous runs
function cleanup()
{
	lvremove -f /dev/mapper/test_* > /dev/null 2>&1
	local LOOPS="$(losetup -a | cut -d: -f1 | tac)"
	if [ -n "$LOOPS" ]; then
		for i in $LOOPS; do
			sync
			losetup -d $i
		done
	fi

	rm -f test_*.img
}

##
# Create a sparse image, then create a loop device from it
function create_loop()
{
	local FUNC="$1"
	local INDEX
	local LOOP
	local IMAGE

	[ -z "$1"    ] && return
	[ -z "$2"    ] || return

	INDEX=${FUNC##*_}
	LOOP="/dev/loop$INDEX"
	IMAGE="$FUNC.img"

	[ -b "$LOOP"       ] || return
	[ -d "$IMAGE_DIR"  ] || return
	[ -n "$IMAGE_SIZE" ] || return

	truncate -s "$IMAGE_SIZE" "$IMAGE"
	[ $? = 0 ] || return

	losetup "$LOOP" "$IMAGE"
	[ $? = 0 ] || return

	rm -f "$IMAGE"
	echo "$LOOP"
}

##
# Create a small subset of a loop device
function sub_loop()
{
	local LOOP
	local START
	local SIZE

	[ -n "$3" ] || return
	[ -z "$4" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	START="$2"
	SIZE="$3"

	START=$((START*1048576))
	SIZE=$((SIZE*1048576))

	losetup --show -f $LOOP --offset $START --sizelimit $SIZE
}


function msdos_init()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel msdos
		quit
	EOF
}

function msdos_print()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1
		unit s
		print free
		quit
	EOF
}

function msdos_create()
{
	local LOOP
	local TYPE
	local START
	local SIZE
	local END

	[ -n "$4" ] || return
	[ -z "$5" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	TYPE="$2"
	START="$3"
	SIZE="$4"

	END="$(((START+SIZE)*2048))"
	START=$((START*2048))

	cat <<-EOF | parted $LOOP > /dev/null
		unit s
		mkpart $TYPE $START $((END-1))
		quit
	EOF
}


function gpt_init()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel gpt
		quit
	EOF
}

function gpt_print()
{
	[ -n "$1" ] || return
	[ -z "$2" ] || return
	[ -b "$1" ] || return

	cat <<-EOF | parted $1
		unit s
		print free
		quit
	EOF
}

function gpt_create()
{
	local LOOP
	local START
	local SIZE
	local END

	[ -n "$3" ] || return
	[ -z "$4" ] || return
	[ -b "$1" ] || return

	LOOP="$1"
	START="$2"
	SIZE="$3"
	END="$((START+SIZE))"

	START=$((START*2048))
	SIZE=$((SIZE*2048))
	END=$((END*2048))

	cat <<-EOF | parted $LOOP > /dev/null
		unit s
		mkpart primary $START $((END-1))
		quit
	EOF
}


