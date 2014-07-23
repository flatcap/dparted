#ifndef _LIST_VISITOR_H_
#define _LIST_VISITOR_H_

#include <sstream>

#include "visitor.h"

class ListVisitor : public Visitor
{
public:
	ListVisitor (void);
	virtual ~ListVisitor();

	virtual bool visit_enter (ContainerPtr& c);
	virtual bool visit_leave (void);

	virtual bool visit (ContainerPtr c);

	void list (void);

protected:
	std::stringstream output;

	int indent = -1;
};

// Simple wrapper
template<class T>
void run_list (std::shared_ptr<T>& top)
{
	ContainerPtr c (top);
	if (!c)
		return;

	ListVisitor lv;
	c->accept (lv);
	lv.list();
}

#endif // _LIST_VISITOR_H_

