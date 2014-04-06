#include <iostream>
#include <ctime>
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
	Gtk::Box vbox;
	Gtk::ButtonBox bbox;
	Gtk::Button action;
	Gtk::TreeView treeview;
	Glib::RefPtr<Gtk::TreeStore> treestore;

	void on_action (void);

private:
	template <class T>
	void add_column (Gtk::TreeModel::ColumnRecord& mod_col_set, Gtk::TreeView::Column* view_col)
	{
		Gtk::TreeModelColumn<T>* mod_col = new Gtk::TreeModelColumn<T>;
		mod_cols.push_back (ModColPtr (mod_col));
		mod_col_set.add (*mod_col);
		view_col->pack_start (*mod_col, false);
	}

	Gtk::TreeModel::ColumnRecord mod_col_set;
	std::vector<ModColPtr> mod_cols;
};

Tree::Tree() :
	vbox (Gtk::ORIENTATION_VERTICAL),
	action ("Action")
{
	set_default_size (400, 200);

	add (vbox);

	vbox.pack_start (treeview);
	vbox.pack_start (bbox, Gtk::PACK_SHRINK);
	bbox.pack_start (action);
	bbox.set_layout(Gtk::BUTTONBOX_END);

	action.signal_clicked().connect (sigc::mem_fun (*this, &Tree::on_action));

	//=====================================================

	Gtk::TreeView::Column* view_col = nullptr;

	//---------------------------

	view_col = Gtk::manage (new Gtk::TreeView::Column ("ID"));
	//add_column<int> (mod_col_set, view_col);
	treeview.append_column (*view_col);

	//---------------------------

	view_col = Gtk::manage (new Gtk::TreeView::Column ("Name"));
	add_column<std::string> (mod_col_set, view_col);
	add_column<std::string> (mod_col_set, view_col);
	treeview.append_column (*view_col);

	//---------------------------

	Gtk::TreeModelColumn<int>* mod_col = new Gtk::TreeModelColumn<int>;
	mod_cols.push_back (ModColPtr (mod_col));
	mod_col_set.add (*mod_col);

	Gtk::CellRendererProgress* rend = Gtk::manage(new Gtk::CellRendererProgress);
	view_col = Gtk::manage (new Gtk::TreeView::Column ("Percentage"));
	view_col->pack_start (*rend, false);
	view_col->add_attribute(rend->property_value(), *mod_col);
	treeview.append_column (*view_col);

	//---------------------------

	treestore = Gtk::TreeStore::create (mod_col_set);
	treeview.set_model (treestore);

	//=====================================================

	show_all_children();
}

void
Tree::on_action (void)
{
	std::vector<std::string> fruit = { "apple", "banana", "cherry", "damson", "elderberry", "fig", "guava", "hawthorn", "ilama", "jackfruit", "kumquat", "lemon", "mango", "nectarine", "olive", "papaya", "quince", "raspberry", "strawberry", "tangerine", "ugli", "vanilla", "wolfberry", "xigua", "yew", "ziziphus" };

	static int init = 0;

	if (init == 4) {
		Gtk::TreeViewColumn *view_col = treeview.get_column(0);

		Gtk::TreeModelColumn<int>* mod_col = new Gtk::TreeModelColumn<int>;

		mod_cols.push_back (ModColPtr (mod_col));
		mod_col_set.add (*mod_col);

		view_col->pack_start (*mod_col, false);

		treestore = Gtk::TreeStore::create (mod_col_set);
		treeview.set_model (treestore);
	}

	treeview.get_column(2)->set_visible ((init < 6) || (init > 12));
	init++;

	Gtk::TreeModel::Row row = *(treestore->append());
	int index = 0;
	row.set_value (index++, fruit[random()%26]);
	row.set_value (index++, fruit[random()%26]);
	row.set_value (index++, random() % 100);
	if (init > 4) {
		row.set_value (index++, random() % 1000);
	}
}

int
main (int argc, char *argv[])
{
	srandom (time (nullptr));

	auto app = Gtk::Application::create (argc, argv, "org.gtkmm.example");
	Tree window;
	return app->run (window);
}


