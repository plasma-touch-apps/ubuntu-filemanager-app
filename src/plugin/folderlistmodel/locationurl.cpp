/**************************************************************************
 *
 * Copyright 2014 Canonical Ltd.
 * Copyright 2014 Carlos J Mazieri <carlos.mazieri@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File: locationurl.cpp
 * Date: 11/03/2014
 */

#include "locationurl.h"

const QString LocationUrl::TrashRootURL("trash:///");
const QString LocationUrl::DiskRootURL("file:///");
const QString LocationUrl::SmbURL("smb://");
const QString LocationUrl::CifsURL("cifs://");
#if 0
QString LocationURL::FishURL("fish:///");
#endif


LocationUrl::LocationUrl()
{
}
