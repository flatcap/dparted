#include <iostream>
#include <gtkmm.h>

typedef std::shared_ptr<Gtk::TreeModelColumnBase> ModColPtr;

/**
 * class Tree
 */
class Tree : public Gtk::Window
{
public:
	Tree();
	virtual ~Tree() = default;

protected:
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

private:
	void add_int_column    (Gtk::TreeModel::ColumnRecord& cols, std::string title);
	void add_string_column (Gtk::TreeModel::ColumnRecord& cols, std::string title);

	std::vector<ModColPtr> mod_cols;
};

void
Tree::add_int_column (Gtk::TreeModel::ColumnRecord& cols, std::string title)
{
	(void) cols;
	(void) title;
}

void
Tree::add_string_column (Gtk::TreeModel::ColumnRecord& cols, std::string title)
{
	(void) cols;
	(void) title;
}

Tree::Tree()
{
	set_default_size (400, 200);

	add (m_TreeView);

	//=====================================================

	Gtk::TreeModel::ColumnRecord cols;
	Gtk::TreeView::Column* col = nullptr;

	//---------------------------

	col = Gtk::manage (new Gtk::TreeView::Column ("ID"));

	Gtk::TreeModelColumn<int>* p_col_id = new Gtk::TreeModelColumn<int>;
	mod_cols.push_back (ModColPtr (p_col_id));
	//cols.add (*mod_cols[0]);
	cols.add (*p_col_id);
	col->pack_start (*p_col_id, false);

	m_TreeView.append_column (*col);

	//---------------------------

	col = Gtk::manage (new Gtk::TreeView::Column ("Name"));

	Gtk::TreeModelColumn<std::string>* p_col_name = new Gtk::TreeModelColumn<std::string>;
	mod_cols.push_back (ModColPtr (p_col_name));
	//cols.add (*mod_cols[1]);
	cols.add (*p_col_name);
	col->pack_start (*p_col_name, false);


	Gtk::TreeModelColumn<std::string>* p_col_surname= new Gtk::TreeModelColumn<std::string>;
	mod_cols.push_back (ModColPtr (p_col_surname));
	//cols.add (*mod_cols[2]);
	cols.add (*p_col_surname);
	col->pack_start (*p_col_surname, true);

	m_TreeView.append_column (*col);

	//---------------------------

	m_refTreeModel = Gtk::TreeStore::create (cols);
	m_TreeView.set_model (m_refTreeModel);

	//=====================================================

	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	std::string jim = "Jim";
	std::string bob = "Bob";
	row.set_value (0, 42);
	row.set_value (1, jim);
	row.set_value (2, bob);

	show_all_children();
}

int
main (int argc, char *argv[])
{
	auto app = Gtk::Application::create (argc, argv, "org.gtkmm.example");
	Tree window;
	return app->run (window);
}


