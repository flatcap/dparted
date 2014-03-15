#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "prop.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

class Object
{
public:
	Object (void);
	~Object();

	std::vector<std::string> get_prop_names (void);
	PPtr get_prop (const std::string& name);
	std::vector<PPtr> get_all_props (void);

	template<typename T>
	void
	declare_prop (const char* owner, const char* name, T& var, const char* desc)
	{
		PPtr pp (new PropVar<T> (owner, name, var, desc));
		props[name] = pp;
	}

	template<typename T>
	void
	declare_prop (const char* owner, const char* name, std::function<T(void)> fn, const char* desc)
	{
		PPtr pp (new PropFn<T> (owner, name, fn, desc));
		props[name] = pp;
	}

	void sub_type (const char* name);

public:
	//properties
	std::string	name;
	std::string	uuid;

	std::string	device;
	dev_t		device_major = 0;
	dev_t		device_minor = 0;

	long		parent_offset = 0;

	long		block_size = 0;
	long		bytes_size = 0;
	long		bytes_used = 0;

	std::vector<std::string> type;

	int seqnum = 123;

protected:
public:
	std::map<std::string,PPtr> props;

	std::string get_uuid_short (void);
	std::string get_device_short (void);
	std::string get_device_major_minor (void);
	std::string get_bytes_size_human (void);
	long   get_bytes_free (void);
	std::string get_bytes_free_human (void);
};

#endif // _OBJECT_H_

