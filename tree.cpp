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
	template <class T>
	void add_column (Gtk::TreeModel::ColumnRecord& cols, Gtk::TreeView::Column* col)
	{
		Gtk::TreeModelColumn<T>* tmc = new Gtk::TreeModelColumn<T>;
		mod_cols.push_back (ModColPtr (tmc));
		cols.add (*tmc);
		col->pack_start (*tmc, false);
	}

	std::vector<ModColPtr> mod_cols;
};

Tree::Tree()
{
	set_default_size (400, 200);

	add (m_TreeView);

	//=====================================================

	Gtk::TreeModel::ColumnRecord cols;
	Gtk::TreeView::Column* col = nullptr;

	//---------------------------

	col = Gtk::manage (new Gtk::TreeView::Column ("ID"));
	add_column<int> (cols, col);
	m_TreeView.append_column (*col);

	//---------------------------

	col = Gtk::manage (new Gtk::TreeView::Column ("Name"));
	add_column<std::string> (cols, col);
	add_column<std::string> (cols, col);
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


