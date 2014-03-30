#include <iostream>
#include <gtkmm/application.h>
#include "tree.h"

int
main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	Tree window;

	return app->run(window);
}

Tree::Tree()
{
	set_default_size(400, 200);

	add(m_TreeView);

	Gtk::TreeModel::ColumnRecord cols;
	Gtk::TreeModelColumn<int> m_col_id;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;

	cols.add(m_col_id);
	cols.add(m_col_name);

	m_refTreeModel = Gtk::TreeStore::create(cols);
	m_TreeView.set_model(m_refTreeModel);

	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	row[m_col_id] = 1;
	row[m_col_name] = "jim";

	m_TreeView.append_column("ID",   m_col_id);
	m_TreeView.append_column("Name", m_col_name);

	show_all_children();
}

