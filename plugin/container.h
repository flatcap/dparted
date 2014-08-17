#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <memory>
#include <string>

typedef std::shared_ptr<class Container> ContainerPtr;

class Container
{
public:
	Container (void);
	virtual ~Container();

	virtual std::string get_name (void);
};

#endif // _CONTAINER_H_

