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

#ifndef _WINDOWS
#include <unistd.h>
#endif
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ZLUnixFileOutputStream.h"
#include "ZLUnixFSManager.h"

ZLUnixFileOutputStream::ZLUnixFileOutputStream(const std::string &name) : myName(name), myHasErrors(false), myFile(0) {
}

ZLUnixFileOutputStream::~ZLUnixFileOutputStream() {
	close();
}

bool ZLUnixFileOutputStream::open() {
	close();
#ifndef _WINDOWS
	myTemporaryName = myName + ".XXXXXX" + '\0';
	if (::mkstemp((char*)myTemporaryName.data()) == 0) {
		return false;
	}
#else
    myTemporaryName = myName + ".XXXXXX" + '\0';
#endif
	myFile = fopen(myTemporaryName.c_str(), "wb");
	return myFile != 0;
}

void ZLUnixFileOutputStream::write(const char *data, size_t len) {
	if (::fwrite(data, 1, len, myFile) != len) {
		myHasErrors = true;
	}
}

void ZLUnixFileOutputStream::write(const std::string &str) {
	if (::fwrite(str.data(), 1, str.length(), myFile) != (size_t)str.length()) {
		myHasErrors = true;
	}
}

void ZLUnixFileOutputStream::close() {
	if (myFile != 0) {
		::fclose(myFile);
		myFile = 0;
		if (!myHasErrors) {
			rename(myTemporaryName.c_str(), myName.c_str());
		}
	}
}
