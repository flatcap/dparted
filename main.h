#ifndef _MAIN_H_
#define _MAIN_H_

#include "plugin.h"

extern register_plugin reg;

bool register_logger     (const std::string& name, LoggerPluginPtr     lp);
bool register_theme      (const std::string& name, ThemePluginPtr      tp);
bool register_filesystem (const std::string& name, FilesystemPluginPtr fp);

#endif // _MAIN_H_

