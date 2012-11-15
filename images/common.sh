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


function usage()
{
	echo "Usage:"
	echo "    ${0##*/} [-d]"
	echo
	grep --color=none "^# .. disk.*" $0
	echo
	false
}

function cleanup()
{
	local i
	local OUR_LOOPS
	local LOSETUP
	local LOOP
	local FILE

	OUR_LOOPS="$(grep -wo 'test_[0-9][0-9]' $0 | sort -u)"
	[ -n "$OUR_LOOPS" ] || return

	vgremove -f $OUR_LOOPS > /dev/null 2>&1

	for i in $OUR_LOOPS; do
		LOSETUP=$(losetup -a | grep "/$i.img")
		[ -n "$LOSETUP" ] || continue

		LOOP=$(sed 's/:.*//' <<< "$LOSETUP")
		if [ -b $LOOP ]; then
			losetup -d $LOOP
		fi
		FILE=$(sed 's/.*(\(.*\))/\1/' <<< "$LOSETUP")
		if [ -f $FILE ]; then
			rm -f $FILE
		fi
	done
}


##
# Create a sparse image, then create a loop device from it
function create_loop()
{
	local IMAGE="$1"
	local LOOP

	[ $# = 1 ] || return

	LOOP=$(losetup --show -f "$IMAGE")
	[ $? = 0 ] || return

	rm -f $IMAGE

	echo "$LOOP"
}

##
# Create a sparse image
function create_image()
{
	local IMAGE="$1.img"

	[ $# = 1 ] || return

	[ -n "$IMAGE_SIZE" ] || return

	rm -f "$IMAGE"
	[ ! -f "$IMAGE" ] || return

	truncate -s "$IMAGE_SIZE" "$IMAGE"
	[ $? = 0 ] || return

	echo "$IMAGE"
}


function msdos_init()
{
	[ $# = 1 ] || return
	[ -b "$1" -o -f "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel msdos
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

	[ $# = 4 ] || return
	[ -b "$1" -o -f "$1" ] || return

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
	[ $# = 1 ] || return
	[ -b "$1" -o -f "$1" ] || return

	cat <<-EOF | parted $1 > /dev/null
		mklabel gpt
		quit
	EOF
}

function gpt_create()
{
	local LOOP
	local START
	local SIZE
	local END

	[ $# = 3 ] || return
	[ -b "$1" -o -f "$1" ] || return

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

function gpt_print()
{
	[ $# = 1 ] || return
	[ -b "$1" -o -f "$1" ] || return

	cat <<-EOF | parted $1
		print
		quit
	EOF
}

