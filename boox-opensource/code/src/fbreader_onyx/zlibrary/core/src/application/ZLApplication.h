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

#ifndef __ZLAPPLICATION_H__
#define __ZLAPPLICATION_H__

#include <string>
#include <vector>
#include <map>
#include <set>

#include <shared_ptr.h>
#include <ZLOptions.h>
#include <ZLTime.h>
#include <ZLResource.h>
#include <ZLMessage.h>
#include <ZLView.h>

#include "onyx/data/bookmark.h"
#include "onyx/data/configuration.h"
#include "onyx/base/base_model.h"


class ZLViewWidget;
class ZLPaintContext;
class ZLKeyBindings;
class ZLToolbar;
class ZLPopupData;
class ZLMenubar;

class ZLApplicationBase {

  protected:
    ZLApplicationBase(const std::string &name);
    ~ZLApplicationBase();
};

class ZLApplication : public ZLApplicationBase {

  public:
    static const std::string MouseScrollUpKey;
    static const std::string MouseScrollDownKey;
    static const std::string NoAction;

  public:
    ZLIntegerOption RotationAngleOption;
    ZLIntegerOption AngleStateOption;

    ZLBooleanOption KeyboardControlOption;

    ZLBooleanOption ConfigAutoSavingOption;
    ZLIntegerRangeOption ConfigAutoSaveTimeoutOption;

    ZLIntegerRangeOption KeyDelayOption;
    ZLStringOption SearchPatternOption;
    ZLBooleanOption SearchForwardOption;

    //ZLStringOption DocumentPathOption;
    ZLStringOption EncodingOption;
    ZLStringOption StateOption;

    QString document_path;
    bool content_ready;
    bool has_thumbnail;

public:
    vbf::Configuration & conf();
    bool loadDocumentOptions(const QString &);
    bool saveDocumentOptions(const QString &);

    vbf::Bookmarks & bookmarks();
    bool addBookmark(const vbf::Bookmark & bookmark);
    bool removeBookmarks(int from, int to);
    bool clearBookmarks();
    bool hasBookmark(size_t from, size_t to);
    void saveBookmarks(const QString &);

    bool isTextSelectionEnabled();
    void enableTextSelection(bool b);

  public:

    class Action {

      public:
        virtual ~Action();
        virtual bool isVisible() const;
        virtual bool isEnabled() const;
        void checkAndRun();
        virtual bool useKeyDelay() const;

      protected:
        virtual void run() = 0;
    };

  protected:
    class RotationAction : public Action {

      public:
        RotationAction(ZLApplication &application);
        bool isVisible() const;
        void run();

      private:
        ZLApplication &myApplication;
    };
    friend class RotationAction;

  public:
    class FullscreenAction : public Action {

      public:
        FullscreenAction(ZLApplication &application);
        void run();

      private:
        ZLApplication &myApplication;
    };
    friend class FullscreenAction;

    class PresentWindowHandler : public ZLMessageHandler {

      public:
        PresentWindowHandler(ZLApplication &application);
        void onMessageReceived(const std::vector<std::string> &arguments);
        const std::string &lastCaller() const;
        void resetLastCaller();

      private:
        ZLApplication &myApplication;
        std::string myLastCaller;
    };

  protected:
    ZLApplication(const std::string &name);

    void addAction(const std::string &actionId, shared_ptr<Action> action);
    void registerPopupData(const std::string &actionId, shared_ptr<ZLPopupData> popupData);

    void setView(shared_ptr<ZLView> view);
    shared_ptr<ZLView> currentView() const;
    ZLView::Angle rotation() const;

    void setFullscreen(bool fullscreen);
    bool isFullscreen() const;
    void quit();

  public:
    virtual ~ZLApplication();
    virtual bool initWindow();

    shared_ptr<ZLPaintContext> context();

    void grabAllKeys(bool grab);
    void trackStylus(bool track);

    void resetWindowCaption();
    void setHyperlinkCursor(bool hyperlink);

    shared_ptr<Action> action(const std::string &actionId) const;
    bool isActionVisible(const std::string &actionId) const;
    bool isActionEnabled(const std::string &actionId) const;
    void doAction(const std::string &actionId);

    virtual shared_ptr<ZLKeyBindings> keyBindings();
    void doActionByKey(const std::string &key);

    virtual bool closeView();
    virtual void openFile(const std::string &fileName);

    virtual bool isViewFinal() const;

    void refreshWindow(bool process = true);
    void presentWindow();

    const std::string &lastCaller() const;
    void resetLastCaller();

    void setVisualParameter(const std::string &id, const std::string &value);
    void setParameterValueList(const std::string &id, const std::vector<std::string> &values);
    const std::string &visualParameter(const std::string &id);

    virtual void loadTreeModelData(std::vector<int> & paragraphs,std::vector<std::string> & titles){};
    virtual void gotoParagraph(int pos){};

  private:
    void createToolbar(int index);

    // TODO: change access rights to private
  public:
    const ZLToolbar &toolbar(int index) const;

  private:
    void createMenubar();
    const ZLMenubar &menubar() const;

  private:
    ZLViewWidget *myViewWidget;
    shared_ptr<ZLView> myInitialView;
    std::map<std::string,shared_ptr<Action> > myActionMap;
    mutable shared_ptr<ZLToolbar> myToolbar;
    mutable shared_ptr<ZLToolbar> myFullscreenToolbar;
    mutable shared_ptr<ZLMenubar> myMenubar;
    shared_ptr<ZLPaintContext> myContext;
    class ZLApplicationWindow *myWindow;
    ZLTime myLastKeyActionTime;
    shared_ptr<ZLMessageHandler> myPresentWindowHandler;
    vbf::Configuration conf_;
    vbf::Bookmarks bookmarks_;
    bool bookmarks_loaded_;
    bool bookmarks_dirty_;

    friend class ZLApplicationWindow;
    friend class ZLMenuVisitor;
};

#endif /* __ZLAPPLICATION_H__ */
