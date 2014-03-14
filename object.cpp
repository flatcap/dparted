#include <iostream>
#include <set>
#include <string>

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
	bytes_size_human
	bytes_used_human
#endif

	name = "table";
	uuid = "0fbb65cd-9514-4297-9da3-205796c81660";
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



