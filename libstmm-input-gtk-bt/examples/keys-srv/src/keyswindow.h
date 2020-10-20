/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   keyswindow.h
 */

#ifndef _KEYS_WINDOW_
#define _KEYS_WINDOW_

#include <string>
#include <memory>

#include <gtkmm.h>
#include <gdkmm.h>

#include <stmm-input/devicemanager.h>
#include <stmm-input-gtk/gtkaccessor.h>

namespace example
{

namespace keyssrv
{

using std::shared_ptr;
using std::weak_ptr;

class KeysWindow : public Gtk::Window
{
public:
	KeysWindow(const Glib::ustring sTitle);
protected:
	// To receive most of the keys, like TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* p0Event) override;
	bool on_key_release_event(GdkEventKey* p0Event) override;
};

} // namespace keyssrv

} // namespace example

#endif /* _KEYS_WINDOW_ */

