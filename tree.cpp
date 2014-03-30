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

	m_refTreeModel = Gtk::TreeStore::create(m_Columns);
	m_TreeView.set_model(m_refTreeModel);

	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	row[m_Columns.m_col_id] = 1;
	row[m_Columns.m_col_name] = "Billy Bob";

	Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(row.children()));
	childrow[m_Columns.m_col_id] = 11;
	childrow[m_Columns.m_col_name] = "Billy Bob Junior";

	childrow = *(m_refTreeModel->append(row.children()));
	childrow[m_Columns.m_col_id] = 12;
	childrow[m_Columns.m_col_name] = "Sue Bob";

	row = *(m_refTreeModel->append());
	row[m_Columns.m_col_id] = 2;
	row[m_Columns.m_col_name] = "Joey Jojo";

	row = *(m_refTreeModel->append());
	row[m_Columns.m_col_id] = 3;
	row[m_Columns.m_col_name] = "Rob McRoberts";

	childrow = *(m_refTreeModel->append(row.children()));
	childrow[m_Columns.m_col_id] = 31;
	childrow[m_Columns.m_col_name] = "Xavier McRoberts";

	m_TreeView.append_column("ID", m_Columns.m_col_id);
	m_TreeView.append_column("Name", m_Columns.m_col_name);

	show_all_children();
}

