/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <sys/types.h>

#include "container_listener.h"
#include "log.h"
#include "mmap.h"
#include "property.h"
#include "question.h"

class Visitor;
struct Action;

typedef std::shared_ptr<class Container> ContainerPtr;
typedef std::weak_ptr  <class Container> ContainerWeak;

#include "transaction.h"

extern std::mutex mutex_write_lock;
extern TransactionPtr txn;

/**
 * class Container - Base class for all containers
 */
class Container
{
public:
	static ContainerPtr create (void);
	virtual ~Container();

	Container& operator= (const Container& c);
	Container& operator= (Container&& c);

	void swap (Container& c);
	friend void swap (Container& lhs, Container& rhs);

	virtual bool accept (Visitor& v);

	ContainerPtr copy (void);

	virtual std::vector<Action> get_actions (void);
	virtual bool perform_action (Action action);

	virtual bool __attribute__((warn_unused_result)) add_child    (ContainerPtr child, bool probe);
	virtual bool __attribute__((warn_unused_result)) delete_child (ContainerPtr child);
	virtual bool __attribute__((warn_unused_result)) move_child   (ContainerPtr child, std::uint64_t offset, std::uint64_t size);

	virtual int      get_fd (void);
	virtual std::uint64_t get_block_size (void);
	virtual std::string   get_device_name (void);
	virtual std::uint64_t get_parent_offset (void);
	virtual std::uint64_t get_device_space (std::map<std::uint64_t, std::uint64_t>& spaces);

	virtual bool can_delete (QuestionPtr) { return false; }
	virtual unsigned int get_count_real_children (void);

	virtual ContainerPtr find (const std::string& uuid);
	//XXX virtual std::vector<ContainerPtr> find_incomplete (void);

	virtual std::uint8_t* get_buffer (std::uint64_t offset, std::uint64_t size);
	virtual void close_buffer (std::uint8_t* buffer, std::uint64_t size);

	virtual bool is_a (const std::string& type);
	virtual bool is_resizeable (void);

	virtual std::vector<ContainerPtr>& get_children (void);

	ContainerPtr get_smart     (void);
	ContainerPtr get_parent    (void);
	ContainerPtr get_top_level (void);
	bool is_top_level (void);

	ContainerPtr backup (void);
	void notify (NotifyType type, ContainerPtr first, ContainerPtr second);

	void add_listener    (const ContainerListenerPtr& m);
	void remove_listener (const ContainerListenerPtr& m);
	int count_listeners (void) { return container_listeners.size(); }

	std::vector<std::string> get_prop_names (void);
	PPtr get_prop (const std::string& name);
	std::vector<PPtr> get_all_props (bool inc_hidden = false);

	template<class T>
	bool __attribute__((warn_unused_result)) add_child (std::shared_ptr<T> child, bool probe);

	void sub_type (const char* name);

	PPtr add_string_prop (const std::string& owner, const std::string& name, const std::string& value);

	static ContainerPtr start_transaction (ContainerPtr& parent, const std::string& desc = "");
	static bool commit_transaction (const std::string& desc = "");
	static void cancel_transaction (void);

	void notify_add    (ContainerPtr child);
	void notify_change (ContainerPtr after);
	void notify_delete (ContainerPtr child);

	std::uint64_t get_unique (void) { return unique_id; }

#ifdef DP_TEST
	ContainerPtr get_nth_child (std::initializer_list<unsigned int> route);
#endif

public:
	// Property helper functions
	std::uint64_t get_absolute_offset            (void);
	std::uint64_t get_bytes_free                 (void);
	std::string   get_device_inherit             (void);
	std::uint64_t get_device_major_inherit       (void);
	std::string   get_device_major_minor         (void);
	std::string   get_device_major_minor_inherit (void);
	std::uint64_t get_device_minor_inherit       (void);
	std::string   get_device_short               (void);
	std::string   get_device_short_inherit       (void);
	std::uint64_t get_file_desc_inherit          (void);
	std::string   get_mmap_addr                  (void);
	std::uint64_t get_mmap_size                  (void);
	std::string   get_name_default               (void);
	std::string   get_object_addr                (void);
	std::uint64_t get_parent_size                (void);
	std::string   get_path_name                  (void);
	std::string   get_path_type                  (void);
	std::int64_t  get_ref_count                  (void);
	std::uint64_t get_top_level_size             (void);
	std::string   get_type                       (void);
	std::string   get_type_long                  (void);
	std::string   get_uuid_short                 (void);

public:
	// properties
	std::string	name;
	std::string	uuid;

	std::string	device;	// These don't belong here, but the alternative is multiple inheritance
	dev_t		device_major = 0;
	dev_t		device_minor = 0;
	int		fd = -1;

	std::uint64_t	parent_offset = 0;

	std::uint64_t	block_size = 0;
	std::uint64_t	bytes_size = 0;
	std::uint64_t	bytes_used = 0;

	ContainerPtr	whole;	//XXX move to protected?

	bool missing = false;

	int seqnum = 0;

protected:
	Container (void);
	Container (const Container& c);
	Container (Container&& c);

	virtual Container* clone (void);

	ContainerWeak self;
	ContainerWeak parent;

	std::vector<std::string> type;

	bool visit_children (Visitor& v);

	friend std::ostream& operator<< (std::ostream& stream, const ContainerPtr& c);
	friend bool operator== (const ContainerPtr& lhs, const ContainerPtr& rhs);
	friend bool exchange (ContainerPtr existing, ContainerPtr replacement);

	std::map<std::string, PPtr> props;

	std::vector<ContainerPtr> children;
	std::recursive_mutex mutex_children;

	std::vector<std::string> more_props;

	template<typename T>
	PPtr declare_prop_var (const char* owner, const char* name, T& var1, const char* desc, int flags, PPtr var2 = nullptr);
	template<typename T>
	PPtr declare_prop_fn (const char* owner, const char* name, std::function<T(void)> fn, const char* desc, int flags, PPtr var2 = nullptr);
	PPtr declare_prop_array (const char* owner, const char* name, std::vector<std::string>& v, unsigned int index, const char* desc, int flags);

	void _add_child (std::vector<ContainerPtr>& vec, ContainerPtr child);
	void txn_add (NotifyType nt, ContainerPtr first, ContainerPtr second);

private:
	MmapPtr	device_mmap;
	std::uint64_t unique_id = 0;
	std::vector<ContainerListenerWeak> container_listeners;
};

template<typename T>
PPtr
Container::declare_prop_var (const char* owner, const char* name, T& var1, const char* desc, int flags, PPtr var2)
{
	return_val_if_fail (owner, nullptr);
	return_val_if_fail (name,  nullptr);
	return_val_if_fail (desc,  nullptr);

	if (flags & BaseProperty::Flags::Size) {		// Create a fake property
		std::string human = std::string (name) + "_human";
		PPtr pvh (new PropVar<T> (owner, human.c_str(), var1, desc, flags & ~BaseProperty::Flags::Dot));
		props[human] = pvh;
		flags &= ~BaseProperty::Flags::Size;		// Turn off the size flag
	}

	PPtr pv (new PropVar<T> (owner, name, var1, desc, flags));
	props[name] = pv;

	if (flags & BaseProperty::Flags::Percent) {		// Create a fake property
		if (!var2) {
			log_debug ("missing var2, can't create percentage");
			return pv;
		}

		if (pv->type != var2->type) {
			log_debug ("types differ, can't create percentage");
			return pv;
		}

		std::string percentage = std::string (name) + "_percentage";
		flags &= ~BaseProperty::Flags::Dot;
		PPtr pvp (new PropPercent (owner, percentage.c_str(), pv, var2, desc, flags));
		props[percentage] = pvp;
	}

	return pv;
}

template<typename T>
PPtr
Container::declare_prop_fn (const char* owner, const char* name, std::function<T(void)> fn, const char* desc, int flags, PPtr var2)
{
	return_val_if_fail (owner, nullptr);
	return_val_if_fail (name,  nullptr);
	return_val_if_fail (desc,  nullptr);

	if (flags & BaseProperty::Flags::Size) {		// Create a fake property
		std::string human (name);
		human += "_human";
		PPtr pp (new PropFn<T> (owner, human.c_str(), fn, desc, flags & ~BaseProperty::Flags::Dot));
		props[human] = pp;
		flags &= ~BaseProperty::Flags::Size;		// Turn off the size flag
	}

	PPtr pf (new PropFn<T> (owner, name, fn, desc, flags));
	props[name] = pf;

	if (flags & BaseProperty::Flags::Percent) {		// Create a fake property
		if (!var2) {
			log_debug ("missing var2, can't create percentage");
			return pf;
		}

		if (pf->type != var2->type) {
			log_debug ("types differ, can't create percentage");
			return pf;
		}

		std::string percentage = std::string (name) + "_percentage";
		flags &= ~BaseProperty::Flags::Dot;
		PPtr pvp (new PropPercent (owner, percentage.c_str(), pf, var2, desc, flags));
		props[percentage] = pvp;
	}

	return pf;
}

template<class T>
bool
Container::add_child (std::shared_ptr<T> child, bool probe)
{
	ContainerPtr c (child);
	return add_child (c, probe);
}


#endif // _CONTAINER_H_

