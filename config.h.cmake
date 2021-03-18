/***************************************************************************
 *   Copyright (C) 2008-2021 by Oleksii S. Malakhov <brezerk@gmail.com>     *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

/*
 * NOTE: DO NOT EDIT THIS FILE!
 *
 * It will be replaced by cmake
 *
 * Edit config.h.cmake instead
 */

#ifndef CONFIG_H

#include <stdlib.h>

#define CONFIG_H

#define D_TREE_COLLAPSE 1
#define D_TREE_EXPAND 2

#define D_PROVIDER_SYSTEM 1
#define D_PROVIDER_WINETRICKS 2

#define D_VIEW_MODE_ICON 0
#define D_VIEW_MODE_LIST 1
#define D_SORT_TYPE_BY_NAME_ASC 1
#define D_SORT_TYPE_BY_NAME_DSC 2
#define D_SORT_TYPE_BY_DATE_ASC 3
#define D_SORT_TYPE_BY_DATE_DSC 4

#define APP_NAME "Q4Wine"
#define APP_SHORT_NAME "q4wine"
#define APP_WEBSITE "q4wine.brezblock.org.ua"
#define APP_BUG_EMAIL "brezerk@gmail.com"

#define APPDB_EXPORT_VERSION "1.0"

#define RUN_STRING_TEMPLATE_DEFAULT   "%CONSOLE_BIN% %CONSOLE_ARGS% %ENV_BIN% %ENV_ARGS% /bin/sh -c \"%WORK_DIR% %SET_NICE% %WINE_BIN% %VIRTUAL_DESKTOP% %PROGRAM_BIN% %PROGRAM_ARGS% 2>&1 \""
#define RUN_STRING_TEMPLATE_X11       "%CONSOLE_BIN% %CONSOLE_ARGS% xinit %ENV_BIN% %ENV_ARGS% /bin/sh -c \"%WORK_DIR% %SET_NICE% %WINE_BIN% %VIRTUAL_DESKTOP% %PROGRAM_BIN% %PROGRAM_ARGS% 2>&1\" -- :1"
#define RUN_STRING_TEMPLATE_SETARCH   "%CONSOLE_BIN% %CONSOLE_ARGS% %ENV_BIN% %ENV_ARGS% /bin/sh -c \"%WORK_DIR% %SET_NICE% setarch i386 -3 -L -B -R %WINE_BIN% %VIRTUAL_DESKTOP% %PROGRAM_BIN% %PROGRAM_ARGS% 2>&1 \""
#define RUN_STRING_TEMPLATE_OPTIRUN   "%CONSOLE_BIN% %CONSOLE_ARGS% %ENV_BIN% %ENV_ARGS% /bin/sh -c \"%WORK_DIR% %SET_NICE% optirun %WINE_BIN% %VIRTUAL_DESKTOP% %PROGRAM_BIN% %PROGRAM_ARGS% 2>&1 \""
#define RUN_STRING_TEMPLATE_PRIMUSRUN "%CONSOLE_BIN% %CONSOLE_ARGS% %ENV_BIN% %ENV_ARGS% /bin/sh -c \"%WORK_DIR% %SET_NICE% primusrun %WINE_BIN% %VIRTUAL_DESKTOP% %PROGRAM_BIN% %PROGRAM_ARGS% 2>&1 \""


#define APPDB_HOSTNAME "q4wine.brezblock.org.ua"
#define BUGS_HOSTNAME "bugs.winehq.org"

#define APPDB_PORT 443

#cmakedefine _OS_LINUX_
#cmakedefine _OS_FREEBSD_
#cmakedefine _OS_DARWIN_

#cmakedefine WITH_ICOUTILS
#cmakedefine WITH_SYSTEM_SINGLEAPP
#cmakedefine WITH_WINEAPPDB

#cmakedefine DEBUG
#cmakedefine RELEASE

#cmakedefine WITH_DBUS

//! process_info struct
typedef struct{
    char *name;
    int pid;
    int nice;
    char *args;
} process_info;

//! process_list struct
typedef struct{
    process_info** procs;
    size_t pnum;
    size_t size;
    size_t block_size;
} process_list;

/*
 * Note that APP_VERS, APP_PREF and APP_BUILD are automatically generated by cmake
 *
 * APP_VERS  -- Application version
 * APP_PREF  -- Application install prefix (Used for trak translations, help and thems files)
 * APP_BUILD -- Application build root (Used for trak translations and themes while RELEASE flag not set)
 */

