
#include "list_visitor.h"
#include "container.h"

ListVisitor::ListVisitor (void)
{
}

ListVisitor::~ListVisitor()
{
}


bool
ListVisitor::visit_enter (ContainerPtr&)
{
	++indent;
	return true;
}

bool
ListVisitor::visit_leave (void)
{
	--indent;
	return true;
}


/**
 * visit (ContainerPtr)
 */
bool
ListVisitor::visit (ContainerPtr c)
{
	if (c->name != "dummy") {
		std::string tabs;
		if (indent > 0) {
			tabs.resize (indent, '\t');
		}
		output << tabs << c << "\n";
	}

	return true;
}


void
ListVisitor::list (void)
{
	printf (output.str().c_str());
}
