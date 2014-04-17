
ContainerPtr
App::scan (std::vector<std::string>& devices)
{
	ContainerPtr top_level = Container::create();

	if (devices.size() > 0) {
		identify (top_level, devices)
	} else {
		discover (top_level)
	}

	return top_level;
}

void
App::identify_device (ContainerPtr parent, std::string& device)
{
	// One check at a time for this device
	Disk::identify (parent, i);
	File::identify (parent, i);
	Loop::identify (parent, i);
	Lvm::identify  (parent, i);
}

void
App::identify (ContainerPtr parent, std::vector<std::string>& devices)
{
	for (auto i : devices) {
		// Examine all the devices in parallel
		std::thread (identify_device(parent, i)).detach();
	}
}

void
App::discover (ContainerPtr parent)
{
	// Check all device types at once
	std::thread (Disk::discover, parent).detach();
	std::thread (File::discover, parent).detach();
	std::thread (Loop::discover, parent).detach();
	std::thread (Lvm::discover,  parent).detach();
}

bool
App::process_queue_item (ContainerPtr item)
{
	//item->get_buffer();

	if (Filesystem::probe (item, buffer, bufsize))
		return true;

	if (Table::probe (item, buffer, bufsize))
		return true;

	if (Misc::probe (item, buffer, bufsize))
		return true;

	return false;
}

bool
App::process_queue (void)
{
	for (auto i : queue) {
		// Examine all the items in parallel
		std::thread (process_queue_item (i).detach());
	}

	return (queue.empty());
}

