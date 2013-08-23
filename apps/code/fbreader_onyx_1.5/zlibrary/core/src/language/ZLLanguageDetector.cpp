/*
 * Copyright (C) 2007-2009 Geometer Plus <contact@geometerplus.com>
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

#include <iostream>

#include <ZLFile.h>
#include <ZLInputStream.h>
#include <ZLDir.h>
#include <ZLUnicodeUtil.h>

#include "ZLLanguageList.h"
#include "ZLLanguageDetector.h"
#include "ZLLanguageMatcher.h"
#include "ZLStatisticsGenerator.h"
#include "ZLStatistics.h"
#include "ZLCharSequence.h"

ZLLanguageDetector::LanguageInfo::LanguageInfo(const std::string &language, const std::string &encoding) : Language(language), Encoding(encoding) {
}

ZLLanguageDetector::ZLLanguageDetector() {
	const ZLFile patternsArchive(ZLLanguageList::patternsDirectoryPath());
	fb::shared_ptr<ZLInputStream> lock = patternsArchive.inputStream();
	fb::shared_ptr<ZLDir> dir = patternsArchive.directory(false);
	if (!dir.isNull()) {
		std::vector<std::string> fileNames;
		dir->collectFiles(fileNames, false);
		for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
			const int index = it->find('_');
			if (index != -1) {
				const std::string language = it->substr(0, index);
				const std::string encoding = it->substr(index + 1);
				fb::shared_ptr<ZLStatisticsBasedMatcher> matcher = new ZLStatisticsBasedMatcher(dir->itemPath(*it), new LanguageInfo(language, encoding));
				myMatchers.push_back(matcher);
			}
		}
	}
}

ZLLanguageDetector::~ZLLanguageDetector() {
}

fb::shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageDetector::findInfo(const char *buffer, size_t length, int matchingCriterion) {
	fb::shared_ptr<LanguageInfo> info;
	ZLMapBasedStatistics bufferStatistics;//= new ZLMapBasedStatistics();
	ZLStatisticsGenerator("\r\n ").generate(buffer, length, 3, bufferStatistics);
	std::string ucs2;
	if( ((unsigned char)buffer[0] == 0xFE) && ((unsigned char)buffer[1] == 0xFF) ) {
		ucs2 = "UCS-2BE";	
	}
	if( ((unsigned char)buffer[0] == 0xFF) && ((unsigned char)buffer[1] == 0xFE) ) {
		ucs2 = "UCS-2LE";	
	}
	for (SBVector::const_iterator it = myMatchers.begin(); it != myMatchers.end(); ++it) {
		if ( (ucs2.empty()) || ( (*it)->info()->Encoding == ucs2 ) ) {
			int criterion = (*it)->criterion(bufferStatistics);
			//std::cerr << (*it)->info()->Language << " " << criterion << "\n";
			if (criterion > matchingCriterion) {
				info = (*it)->info();
				matchingCriterion = criterion;
			}
		}
	}
	return info;
}