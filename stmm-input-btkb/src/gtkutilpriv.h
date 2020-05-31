/*
 * File:   gtkutilpriv.h
 *
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */

#ifndef STMG_GTK_UTIL_PRIV_H
#define STMG_GTK_UTIL_PRIV_H

#include <gtkmm.h>

namespace stmi
{

void addSeparator(Gtk::Box* p0Box, int32_t nHeightPix) noexcept;
void addSeparator(Gtk::Box* p0Box, int32_t nHeightPix, bool bExpand) noexcept;
void addBigSeparator(Gtk::Box* p0Box) noexcept;
void addBigSeparator(Gtk::Box* p0Box, bool bExpand) noexcept;
void addSmallSeparator(Gtk::Box* p0Box) noexcept;
void addSmallSeparator(Gtk::Box* p0Box, bool bExpand) noexcept;

} // namespace stmi

#endif	/* STMG_GTK_UTIL_PRIV_H */

