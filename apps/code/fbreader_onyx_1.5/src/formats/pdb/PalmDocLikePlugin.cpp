/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <ZLFile.h>

#include "PdbPlugin.h"
#include "PalmDocStream.h"
#include "PalmDocLikeStream.h"
#include <iostream>

bool PalmDocLikePlugin::providesMetaInfo() const {
	return true;
}

fb::shared_ptr<ZLInputStream> PalmDocLikePlugin::createStream(ZLFile &file) const {
	std::cerr << "Create PalmDoc stream specified by PalmDocLikePlugin...\n";
	return new PalmDocStream(file);
}

const std::string &PalmDocLikePlugin::tryOpen(const std::string &path) const {
	ZLFile file(path);
	PalmDocStream stream(file);
	stream.open();
	return stream.error();
}
