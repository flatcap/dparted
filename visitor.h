#ifndef _VISITOR_H_
#define _VISITOR_H_

#include <memory>

#include "container.h"

class Visitor
{
public:
	virtual bool visit_enter (ContainerPtr&) { return true; }
	virtual bool visit_leave (void)          { return true; }

	virtual bool visit (ContainerPtr c) = 0;
};


#endif // _VISITOR_H_

