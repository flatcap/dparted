#ifndef _TREE_H_
#define _TREE_H_

#include <gtkmm.h>

class Tree : public Gtk::Window
{
public:
	Tree();
	virtual ~Tree() = default;

protected:
	Gtk::TreeModel::ColumnRecord m_Columns;

	Gtk::TreeModelColumn<int> m_col_id;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;

	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
};

#endif // _TREE_H_

