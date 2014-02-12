/* Copyright (c) 2014 Richard Russon (FlatCap)
 *
 * This file is part of DParted.
 *
 * DParted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DParted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DParted.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/menubar.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/spinbutton.h>
#include <giomm/simpleactiongroup.h>

#include "tree_view.h"
#include "drawing_area.h"

class Container;

/**
 * class Window
 */
class Window : public Gtk::ApplicationWindow
{
public:
	Window();
	virtual ~Window();

	void set_data (GfxContainerPtr c);
	bool set_focus (GfxContainerPtr cont);
	GfxContainerPtr get_focus (void);

	void load_config (const std::string& filename);
	void load_theme  (const std::string& filename);
	void load_disk   (const std::string& filename);

	void set_geometry (int x, int y, int w, int h);

protected:
	Gtk::Box		outer_box;	//XXX dynamically create the ones we don't care about?
	Gtk::MenuBar*		menubar = nullptr;	//do not delete
	Gtk::Toolbar*		toolbar = nullptr;	//do not delete
	Gtk::EventBox		eventbox;
	Gtk::ScrolledWindow	scrolledwindow;
	Gtk::Box		inner_box;
	DrawingArea		drawingarea;
	TreeView		treeview;
	Gtk::Statusbar		statusbar;

	// MENU
	void init_menubar (Gtk::Box& box);
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	Glib::RefPtr<Gio::SimpleAction> m_refChoice;
	Glib::RefPtr<Gio::SimpleAction> m_refChoiceOther;
	Glib::RefPtr<Gio::SimpleAction> m_refToggle;

	Glib::RefPtr<Gio::SimpleAction> m_refViewGfx;
	Glib::RefPtr<Gio::SimpleAction> m_refViewTree;
	Glib::RefPtr<Gio::SimpleAction> m_refViewToolbar;
	Glib::RefPtr<Gio::SimpleAction> m_refViewStatus;

	void on_menu_choices(const Glib::ustring& parameter);
	void on_menu_choices_other(int parameter);
	void on_menu_file_new_generic (void);
	void on_menu_file_quit (void);
	void on_menu_others (void);
	void on_menu_toggle (void);
	void on_menu_view (int option);

	Gtk::Menu m_fake_menu;

	bool on_mouse_click (GdkEventButton* event);

	void on_keypress (int modifier, int key);
	void init_shortcuts (void);
	void init_actions (void);

	void my_realize (void);
	void my_show (void);
	bool my_idle (void);

	ContainerPtr m_c;
	GfxContainerPtr m_g;

	GfxContainerPtr focus;
	std::vector<GfxContainerPtr> selection;

	void on_action_edit_cut               (void);
	void on_action_edit_copy              (void);
	void on_action_edit_paste             (void);
	void on_action_edit_paste_special     (void);
	void on_action_edit_undo              (void);
	void on_action_edit_redo              (void);
	void on_action_edit_clear_all_ops     (void);
	void on_action_edit_apply_all_ops     (void);
	void on_action_edit_find              (void);
	void on_action_edit_find_next         (void);
	void on_action_edit_find_previous     (void);
	void on_action_edit_preferences       (void);
	void on_action_view_toolbar           (void);
	void on_action_view_graphics          (void);
	void on_action_view_tree_view         (void);
	void on_action_view_status_bar        (void);
	void on_action_view_log               (void);
	void on_action_view_pending_actions   (void);
	void on_action_view_fullscreen        (void);
	void on_action_view_refresh           (void);
	void on_action_view_reload            (void);
	void on_action_view_theme             (void);
	void on_action_create_filesystem      (void);
	void on_action_create_partition       (void);
	void on_action_create_table           (void);
	void on_action_create_encryption      (void);
	void on_action_create_lvm_volume      (void);
	void on_action_create_subvolume       (void);
	void on_action_create_snapshot        (void);
	void on_action_delete_filesystem      (void);
	void on_action_delete_partition       (void);
	void on_action_delete_table           (void);
	void on_action_delete_encryption      (void);
	void on_action_delete_lvm_volume      (void);
	void on_action_delete_subvolume       (void);
	void on_action_delete_snapshot        (void);
	void on_action_format_wipe            (void);
	void on_action_format_filesystem      (void);
	void on_action_format_btrfs           (void);
	void on_action_format_partition_type  (void);
	void on_action_format_table           (void);
	void on_action_manage_properties      (void);
	void on_action_manage_label           (void);
	void on_action_manage_uuid            (void);
	void on_action_manage_flags           (void);
	void on_action_manage_parameters      (void);
	void on_action_filesystem_check       (void);
	void on_action_filesystem_defragment  (void);
	void on_action_filesystem_rebalance   (void);
	void on_action_filesystem_resize_move (void);
	void on_action_filesystem_mount       (void);
	void on_action_filesystem_umount      (void);
	void on_action_filesystem_swap_on     (void);
	void on_action_filesystem_swap_off    (void);
	void on_action_filesystem_usage       (void);
	void on_action_group_resize           (void);
	void on_action_group_split            (void);
	void on_action_group_merge            (void);
	void on_action_group_add_stripe       (void);
	void on_action_group_remove_stripe    (void);
	void on_action_group_add_mirror       (void);
	void on_action_group_remove_mirror    (void);
	void on_action_group_break_mirror     (void);
	void on_action_group_add_raid         (void);
	void on_action_group_remove_raid      (void);

private:
};


#endif // _WINDOW_H_

