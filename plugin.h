#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#define DP_PLUGIN_MAGIC		0xD93C38D3
#define DP_PLUGIN_VERSION	0x00000001

typedef struct
{
	std::uint32_t magic;
	std::uint32_t version;
	void initialise   (register_plugin* reg);
	void uninitialise (void);
} dparted_plugin;

typedef struct
{
	bool register_logger     (LoggerPluginPtr     lp);
	bool register_theme      (ThemePluginPtr      tp);
	bool register_filesystem (FilesystemPluginPtr fp);
} register_plugin;

dparted_plugin get_dparted_plugin_info (void);

typedef struct
{
	Gdk::RGBA get_colour (const std::string& name);
	Glib::RefPtr<Gdk::Pixbuf> get_icon (const std::string& name);
} theme_plugin;

#endif // _PLUGIN_H_


