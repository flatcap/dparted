#ifndef _DPARTED_H_
#define _DPARTED_H_

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/menubar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/statusbar.h>

#include <gtkmm.h>

#include "drawingarea.h"
#include "treeview.h"

class DPContainer;

class DParted : public Gtk::Window
{
public:
	DParted();
	virtual ~DParted();

	void set_data (DPContainer *c);

protected:
	// GUI
	Gtk::Grid		grid;
	Gtk::MenuBar		menubar;
	//Gtk::Toolbar		toolbar;
	DrawingArea		drawingarea;
	TreeView		treeview;
	//Gtk::Statusbar		statusbar;

	Gtk::ScrolledWindow	scrolledwindow;

	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	Glib::RefPtr<Gtk::RadioAction> m_refChoiceOne, m_refChoiceTwo;

	//Signal handlers:
	void on_menu_file_new_generic();
	void on_menu_file_quit();
	void on_menu_others();

	void on_menu_choices_one();
	void on_menu_choices_two();

	DPContainer *m_c;

private:
	void init_menubar (void);
#if 0
	void init_toolbar (void);
	void init_scrolledwindow (void);
#endif
};

#endif // _DPARTED_H_

