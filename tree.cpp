#include <iostream>
#include <gtkmm.h>

class Tree : public Gtk::Window
{
public:
	Tree();
	virtual ~Tree() = default;

protected:
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::TreeStore> m_TreeStore;
};

int
main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
	Tree window;
	return app->run(window);
}


typedef std::shared_ptr<Gtk::TreeModelColumnBase> ModColPtr;
std::vector<ModColPtr> mod_cols;

Tree::Tree()
{
	set_default_size(400, 200);

	add(m_TreeView);

	Gtk::TreeModel::ColumnRecord cols;

	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<int>));		// 0 id
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));	// 1 name
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));	// 2 family

	cols.add(*mod_cols[0]);	// id
	cols.add(*mod_cols[1]); // name
	cols.add(*mod_cols[2]); // family

	m_TreeStore = Gtk::TreeStore::create(cols);
	m_TreeView.set_model(m_TreeStore);

	Gtk::CellRenderer* pCellRenderer = nullptr;
	Gtk::TreeView::Column* col = nullptr;

	pCellRenderer = manage(new Gtk::CellRendererText());
	col = Gtk::manage (new Gtk::TreeView::Column ("ID", *pCellRenderer));
	col->set_renderer(*pCellRenderer, *mod_cols[0]);
	m_TreeView.append_column (*col);

	pCellRenderer = manage(new Gtk::CellRendererText());
	col = Gtk::manage (new Gtk::TreeView::Column ("Name", *pCellRenderer));
	col->set_renderer(*pCellRenderer, *mod_cols[1]);
	m_TreeView.append_column (*col);

	pCellRenderer = manage(new Gtk::CellRendererText());
	col = Gtk::manage (new Gtk::TreeView::Column ("Surname", *pCellRenderer));
	col->set_renderer(*pCellRenderer, *mod_cols[2]);
	m_TreeView.append_column (*col);

	Gtk::TreeModel::Row row = *(m_TreeStore->append());		// iterator
	std::string name    = "James";
	std::string surname = "Cagney";

	row->set_value (0, 42);
	row->set_value (1, name);
	row->set_value (2, surname);

	show_all_children();
}

