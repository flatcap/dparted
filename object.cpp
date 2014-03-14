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
}

Object::~Object()
{
}


#ifdef RAR
std::string
Object::get_prop (const std::string& propname)
{
	if (propname == "name") {
		return name;
	} else if (propname == "uuid") {
		return uuid;
	} else if (propname == "uuid_short") {
		std::string uuid_short = uuid;

		if ((uuid_short.size() > 8) && (uuid_short[0] != '/')) {
			uuid_short = uuid_short.substr (0, 6) + "...";
		}

		return uuid_short;
	} else if (propname == "device") {
		return get_device_name();
	} else if (propname == "device_short") {
		std::string d = get_device_name();
		size_t pos = d.find_last_of ('/');
		if (pos != std::string::npos) {
			d.erase (0, pos+1);
		}
		return d;
	} else if (propname == "parent_offset") {
		return std::to_string (parent_offset);
	} else if (propname == "block_size") {
		return std::to_string (block_size);
	} else if (propname == "bytes_size") {
		return std::to_string (bytes_size);
	} else if (propname == "bytes_size_human") {
		return get_size (bytes_size);
	} else if (propname == "bytes_used") {
		return std::to_string (bytes_used);
	} else if (propname == "bytes_used_human") {
		return get_size (bytes_used);
	}

	return propname;
}
#endif

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



