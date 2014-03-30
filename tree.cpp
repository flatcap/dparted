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

#if 1
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<int>));		// 0 id
	mod_cols.push_back (ModColPtr (new Gtk::TreeModelColumn<std::string>));	// 1 name
#else
	Gtk::TreeModelColumn<int>     m_col_id;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
#endif

#if 1
	cols.add(*mod_cols[0]);	// id
	cols.add(*mod_cols[1]); // name
#else
	cols.add(m_col_id);
	cols.add(m_col_name);
#endif

	m_TreeStore = Gtk::TreeStore::create(cols);
	m_TreeView.set_model(m_TreeStore);

	Gtk::TreeModel::Row row = *(m_TreeStore->append());		// iterator
	std::string james = "jim";
#if 1
	row->set_value (0, 42);		// id
	row->set_value (1, james);      // name
#else
	row[m_col_id]   = 42;
	row[m_col_name] = james;
#endif

#if 1
	// m_TreeView.append_column("ID",   *mod_cols[0]);
	// m_TreeView.append_column("Name", *mod_cols[1]);

	Gtk::CellRenderer* pCellRenderer = nullptr;
	Gtk::TreeView::Column* col = nullptr;

	//Add the TreeView's view columns:
	pCellRenderer = manage(new Gtk::CellRendererText());
	col = Gtk::manage (new Gtk::TreeView::Column ("ID", *pCellRenderer));
	col->set_renderer(*pCellRenderer, *mod_cols[0]);
	m_TreeView.append_column (*col);

	pCellRenderer = manage(new Gtk::CellRendererText());
	col = Gtk::manage (new Gtk::TreeView::Column ("Name", *pCellRenderer));
	col->set_renderer(*pCellRenderer, *mod_cols[1]);
	m_TreeView.append_column (*col);
#else
	m_TreeView.append_column("ID",   m_col_id);
	m_TreeView.append_column("Name", m_col_name);
#endif

	show_all_children();
}

