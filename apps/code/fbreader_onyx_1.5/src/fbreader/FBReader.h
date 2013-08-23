// -*- mode: c++; c-basic-offset: 4; -*-

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

#ifndef __FBREADER_H__
#define __FBREADER_H__

#include <string>
#include <map>

#include <ZLOptions.h>
#include <ZLTime.h>
#include <ZLView.h>
#include <ZLApplication.h>
#include <ZLKeyBindings.h>

#include "../database/booksdb/DBBook.h"
class ZLViewWidget;
class ZLMessageHandler;

class BookModel;
class BookTextView;

class FBReader : public ZLApplication {
  public:
    // returns true if description was found or error message was shown
    static bool createDescription(const std::string &fileName, fb::shared_ptr<DBBook> &book);

    static const std::string PageIndexParameter;

  public:
    enum ViewMode {
        UNDEFINED_MODE = 0,
        BOOK_TEXT_MODE = 1 << 0,
        FOOTNOTE_MODE = 1 << 1,
        CONTENTS_MODE = 1 << 2,
        BOOKMARKS_MODE = 1 << 3,
		BOOK_COLLECTION_MODE = 1 << 4,
		NET_LIBRARY_MODE = 1 << 5,
        ALL_MODES = 0x3F
    };

    struct ScrollingOptions {
        ScrollingOptions(const std::string &groupName, long delayValue, long modeValue, long linesToKeepValue, long linesToScrollValue, long percentToScrollValue);
        ZLIntegerRangeOption DelayOption;
        ZLIntegerOption ModeOption;
        ZLIntegerRangeOption LinesToKeepOption;
        ZLIntegerRangeOption LinesToScrollOption;
        ZLIntegerRangeOption PercentToScrollOption;
    };

  public:
    ZLBooleanOption QuitOnCancelOption;

    ScrollingOptions LargeScrollingOptions;
    ScrollingOptions SmallScrollingOptions;
    ScrollingOptions MouseScrollingOptions;
    ScrollingOptions TapScrollingOptions;
    ZLBooleanOption EnableTapScrollingOption;
    ZLBooleanOption TapScrollingOnFingerOnlyOption;

    ZLBooleanOption UseSeparateBindingsOption;

    ZLBooleanOption EnableSingleClickDictionaryOption;

  public:
    FBReader(const std::string &bookToOpen);
    ~FBReader();

    void setMode(ViewMode mode);
    ViewMode mode() const;

  private:
    bool initWindow();

    void clearTextCaches();

    void restorePreviousMode();

    bool closeView();
    std::string helpFileName(const std::string &language) const;
    void openFile(const std::string &fileName);

    bool isViewFinal() const;

  public:
    fb::shared_ptr<ZLKeyBindings> keyBindings();
    fb::shared_ptr<ZLKeyBindings> keyBindings(ZLView::Angle angle);

    bool isDictionarySupported() const;
    void openInDictionary(const std::string &word);

    void tryShowFootnoteView(const std::string &id, const std::string &type);
    BookTextView &bookTextView() const;
    void showBookTextView();
	void openBook(fb::shared_ptr<DBBook> book);

  private:
	void openBookInternal(fb::shared_ptr<DBBook> book);
    friend class OpenBookRunnable;
    void rebuildCollectionInternal();
    friend class RebuildCollectionRunnable;
    friend class OptionsApplyRunnable;

  private:
    ViewMode myMode;
    ViewMode myPreviousMode;

    fb::shared_ptr<ZLView> myFootnoteView;
    fb::shared_ptr<ZLView> myBookTextView;
    fb::shared_ptr<ZLView> myContentsView;

    ZLTime myLastScrollingTime;

    BookModel *myModel;

    fb::shared_ptr<ZLKeyBindings> myBindings0;
    fb::shared_ptr<ZLKeyBindings> myBindings90;
    fb::shared_ptr<ZLKeyBindings> myBindings180;
    fb::shared_ptr<ZLKeyBindings> myBindings270;

    std::string myBookToOpen;
    bool myBookAlreadyOpen;

    fb::shared_ptr<ZLMessageHandler> myOpenFileHandler;

    enum {
        RETURN_TO_TEXT_MODE,
        UNFULLSCREEN
    } myActionOnCancel;

friend class OpenFileHandler;

    friend class OptionsDialog;
    friend class FBView;

    friend class ShowHelpAction;
    friend class ShowContentsAction;
    friend class ShowBookInfoAction;

    friend class UndoAction;

    friend class SearchAction;
    friend class SearchPatternAction;
    friend class ClearSearchResultAction;
    friend class FindNextAction;
    friend class FindPreviousAction;
    friend class ScrollingAction;
    friend class ChangeFontSizeAction;
    friend class UpdateOptionsAction;
    friend class CancelAction;

    friend class QuitAction;
    friend class GotoNextTOCSectionAction;
    friend class GotoPreviousTOCSectionAction;
    friend class SelectionAction;
    friend class FBFullscreenAction;
};

#endif /* __FBREADER_H__ */
