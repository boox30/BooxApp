/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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


#include "../DBRunnables.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"


LoadSeriesRunnable::LoadSeriesRunnable(DBConnection &connection) {
	myLoadSeries      = SQLiteFactory::createCommand(BooksDBQuery::LOAD_SERIES, connection, "@book_id", DBValue::DBINT);
}

bool LoadSeriesRunnable::run() {
	mySeriesName = "";
	myNumberInSeries = 0;

	((DBIntValue &) *myLoadSeries->parameter("@book_id").value()) = myBookId;
	fb::shared_ptr<DBDataReader> reader = myLoadSeries->executeReader();
	if (reader.isNull()) {
		return false;
	}

	if (!reader->next()) {
		reader->close();
		return true;
	}

	int numType = reader->type(1);
	if (reader->type(0) != DBValue::DBTEXT || (numType != DBValue::DBINT && numType != DBValue::DBNULL)) {
		reader->close();
		return false;
	}
	mySeriesName = reader->textValue(0);
	if (numType == DBValue::DBINT) {
		myNumberInSeries = reader->intValue(1);
	} else {
		myNumberInSeries = 0;
	}
	reader->close();
	return true;
}

