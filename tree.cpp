#include <iostream>
#include <gtkmm.h>

class Tree : public Gtk::Window
{
public:
	Tree();
	virtual ~Tree() = default;

protected:
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
};

Tree::Tree()
{
	set_default_size (400, 200);

	add (m_TreeView);

	//-----------------------------------------------------

	Gtk::TreeModel::ColumnRecord cols;

	Gtk::TreeModelColumn<int> m_col_id;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;

	cols.add (m_col_id);
	cols.add (m_col_name);

	m_refTreeModel = Gtk::TreeStore::create (cols);
	m_TreeView.set_model (m_refTreeModel);

	m_TreeView.append_column ("ID",   m_col_id);
	m_TreeView.append_column ("Name", m_col_name);

	//-----------------------------------------------------

	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	std::string jim = "Jim";
	row.set_value (0, 42);
	row.set_value (1, jim);

	show_all_children();
}

int
main (int argc, char *argv[])
{
	auto app = Gtk::Application::create (argc, argv, "org.gtkmm.example");
	Tree window;
	return app->run (window);
}

