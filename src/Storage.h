#ifndef INC_2020_STORAGE_H
#define INC_2020_STORAGE_H

#include <string>

#include "scene/Settings.h"

namespace storage { 

    /*
     * All settings are stored under a per-user settings directory.
     * (Default: ~/.config/spatzsim/). Use this function to obtain
     * the absolute path of this settings directory as a string for
     * the current user.
     */
    std::string getXDGSettingsDirectory();

    /*
     * Call this function at program startup to create the
     * default settings directory (~/.config/spatzsim/).
     */
    bool createXDGSettingsDirectory();

    /*
     * The idea of this storage implementation is that,
     * there are two functions: load and save. To store 
     * or read any object on disk you just call these functions.
     * And depending on the object type an appropriate implementation
     * (see Storage.cpp) is called.
     *
     * So instead of separate load functions on different
     * objects, there is a uniform interface for loading
     * and saving that can be access through a single header.
     */

    template <typename T>
    bool load(T& t, std::string path);

    template <typename T>
    bool save(T& t, std::string path);

    /*
     * Note, that it is totally valid to implement overloads
     * when objects require a different set of parameters.
     */

    /*
     * This tries to load or save the settings file from or to the
     * XDG config directory. (see getXDGSettingsDirectory())
     */
    bool load(Settings& settings);
    bool save(Settings& settings);
}

#endif
