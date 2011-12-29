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


