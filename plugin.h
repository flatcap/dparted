#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <string>
#include <memory>
#include <functional>

#define PLUGIN_MAGIC		0xD93C38D3
#define PLUGIN_VERSION	0x00000001

struct ThemePlugin
{
	std::function<std::string(std::string)> get_colour;
	std::function<std::string(std::string)> get_icon;
};

struct LoggerPlugin
{
	std::function<std::string(std::string)> get_colour;
	std::function<std::string(std::string)> get_icon;
};

struct FilesystemPlugin
{
	std::function<std::string(std::uint8_t* buffer, std::uint64_t size)> identify;
};

typedef std::shared_ptr<struct LoggerPlugin>     LoggerPluginPtr;
typedef std::shared_ptr<struct ThemePlugin>      ThemePluginPtr;
typedef std::shared_ptr<struct FilesystemPlugin> FilesystemPluginPtr;

// plugin get_plugin_info (void);

typedef struct
{
	std::function<bool(const std::string&, LoggerPluginPtr)>     register_logger;
	std::function<bool(const std::string&, ThemePluginPtr)>      register_theme;
	std::function<bool(const std::string&, FilesystemPluginPtr)> register_filesystem;
} register_plugin;

typedef struct
{
	std::uint32_t magic;
	std::uint32_t version;
	std::function<int(register_plugin* reg)> initialise;
	std::function<void(void)>                uninitialise;
} plugin;

#endif // _PLUGIN_H_


