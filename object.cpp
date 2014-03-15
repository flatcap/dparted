#include <iostream>
#include <set>
#include <string>
#include <cmath>

#include "object.h"

Object::Object (void)
{
	const char* me = "Object";

	declare_prop (me, "name",          name,          "desc of name");
	declare_prop (me, "uuid",          uuid,          "desc of uuid");
	declare_prop (me, "device",        device,        "desc of device");
	declare_prop (me, "device_major",  device_major,  "desc of device_major");
	declare_prop (me, "device_minor",  device_minor,  "desc of device_minor");
	declare_prop (me, "parent_offset", parent_offset, "desc of parent_offset");
	declare_prop (me, "block_size",    block_size,    "desc of block_size");
	declare_prop (me, "bytes_size",    bytes_size,    "desc of bytes_size");
	declare_prop (me, "bytes_used",    bytes_used,    "desc of bytes_used");

#if 0
	uuid_short
	device_short
	device_major_minor
	bytes_size_human
	bytes_used_human
#endif

	name = "table";
	uuid = "b23896a2-2023-4039-bb99-87fa92efe821";
	device = "/dev/mapper/rich-table";
	device_major = 253;
	device_minor = 35;
	parent_offset = 4194304;
	block_size = 4096;
	bytes_size = 104857600;
	bytes_used = 45088768;
}

Object::~Object()
{
}


std::vector<std::string>
Object::get_prop_names (void)
{
	std::vector<std::string> names;
	for (auto p : props) {
		names.push_back (p.second->name);
	}

	return names;
}

PPtr
Object::get_prop (const std::string& name)
{
	//std::cout << "get_prop: " << props.count (name) << std::endl;
	//XXX check exists, otherwise throw
	return props[name];
}

std::vector<PPtr>
Object::get_all_props (void)
{
	std::vector<PPtr> vv;

	for (auto p : props) {
		//XXX what's the magic C++11 way of doing this?
		vv.push_back (p.second);
	}

	return vv;
}


std::string
get_size (long size)
{
	//XXX do this without log2?  use gcc's __builtin_clzl?
	char buffer[64];
	double power = log2 ((double) llabs (size)) + 0.5;
	const char* suffix = "";
	double divide = 1;

	if (power < 10) {
		suffix = "   B";
		divide = 1;
	} else if (power < 20) {
		suffix = " KiB";
		divide = 1024;
	} else if (power < 30) {
		suffix = " MiB";
		divide = 1048576;
	} else if (power < 40) {
		suffix = " GiB";
		divide = 1073741824;
	} else if (power < 50) {
		suffix = " TiB";
		divide = 1099511627776;
	} else if (power < 60) {
		suffix = " PiB";
		divide = 1125899906842624;
	}
	sprintf (buffer, "%0.3g%s", (double) size/divide, suffix);
	return buffer;
}


std::string
Object::get_uuid_short (void)
{
	std::string u = uuid;
	size_t pos = uuid.find_first_of ("-:");

	if (pos != std::string::npos) {
		u = uuid.substr (0, pos);
	}

	return u;
}

std::string
Object::get_device_short (void)
{
	std::string d = device;
	size_t pos = device.find ("/dev");

	if (pos != std::string::npos) {
		d = device.substr (pos+5);
	}

	return d;
}

std::string
Object::get_device_major_minor (void)
{
	if ((device_major == 0) && (device_minor == 0))
		return "";

	return std::to_string (device_major) + ":" + std::to_string (device_minor);
}

std::string
Object::get_bytes_size_human (void)
{
	return get_size (bytes_size);
}

std::string
Object::get_bytes_free_human (void)
{
	return get_size (bytes_size - bytes_used);
}


