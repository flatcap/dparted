#ifndef _DISK_H_
#define _DISK_H_

#include <memory>
#include <string>

#include "container.h"

typedef std::shared_ptr<class Disk> DiskPtr;

class Disk : public Container
{
public:
	Disk (void);
	virtual ~Disk();

	virtual std::string get_name (void);
};

#endif // _DISK_H_

