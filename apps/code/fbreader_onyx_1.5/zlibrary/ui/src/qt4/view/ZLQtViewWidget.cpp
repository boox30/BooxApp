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

#include <algorithm>

#include <QtGui/QLayout>
#include <QtGui/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"
#include "ZLTextStyle.h"
#include "ZLTextView.h"

#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/data/configuration.h"
#include "onyx/ui/tree_view_dialog.h"

using namespace cms;
using namespace vbf;

static const int BEFORE_SEARCH = 0;
static const int IN_SEARCHING  = 1;

class MyQScrollBar : public QScrollBar {

public:
    MyQScrollBar(Qt::Orientation orientation, QWidget *parent) : QScrollBar(orientation, parent) {
    }

private:
    void mouseMoveEvent(QMouseEvent *event) {
        if (orientation() == Qt::Vertical) {
            const int y = event->y();
            if ((y <= 0) || (y >= height())) {
                return;
            }
        } else {
            const int x = event->x();
            if ((x <= 0) || (x >= width())) {
                return;
            }
        }
        QScrollBar::mouseMoveEvent(event);
    }
};
static const int SHIFT = 10;

ZLQtViewWidget::Widget::Widget(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
    //setBackgroundMode(NoBackground);
}

QWidget * ZLQtViewWidget::addStatusBar()
{
    status_bar_ = new StatusBar(widget(), ui::MENU|PROGRESS|MESSAGE|BATTERY);
    connect(status_bar_, SIGNAL(menuClicked()), this, SLOT(popupMenu()));
    connect(status_bar_, SIGNAL(progressClicked(const int, const int)),
        this, SLOT(onProgressClicked(const int, const int)));
    return status_bar_;
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLApplication *application)
: ZLViewWidget((ZLView::Angle)application->AngleStateOption.value())
, myApplication(application)
, status_bar_(0)
, sys_status_(sys::SysStatus::instance())
, enable_text_selection_(false)
, conf_stored_(false)
{
    myFrame = new QWidget(parent);
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    myFrame->setLayout(layout);
    myQWidget = new Widget(myFrame, *this);
    layout->addWidget(myQWidget, 0, 0);
    layout->addWidget(addStatusBar(), 1, 0);

    // Setup connection
    connect(&sys_status_, SIGNAL(sdCardChangedSignal(bool)), this, SLOT(onSdCardChanged(bool)));
    connect(&sys_status_, SIGNAL(mountTreeSignal(bool, const QString &)),
            this, SLOT(handleMountTreeEvent(bool, const QString &)));
    connect(&sys_status_, SIGNAL(aboutToShutdown()), this, SLOT(onAboutToShutdown()));
    connect(&sys_status_, SIGNAL(wakeup()), this, SLOT(onWakeup()));
    connect(&sys_status_, SIGNAL(musicPlayerStateChanged(int)),
            this, SLOT(onMusicPlayerStateChanged(int)));


    // Load conf.
    loadConf();
}

ZLQtViewWidget::~ZLQtViewWidget()
{
    // Should release them as the parent widget can release the
    // resouce. So scoped_ptr should not release them again.
    dict_widget_.release();
    tts_widget_.release();
    search_widget_.release();
}

void ZLQtViewWidget::trackStylus(bool track) {
    myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::Widget::paintEvent(QPaintEvent*) {
    ZLQtPaintContext &context = (ZLQtPaintContext&)myHolder.view()->context();
    switch (myHolder.rotation()) {
        default:
            context.setSize(width(), height());
            break;
        case ZLView::DEGREES90:
        case ZLView::DEGREES270:
            context.setSize(height(), width());
            break;
    }
    myHolder.view()->paint();
    QPainter realPainter(this);
    switch (myHolder.rotation()) {
        default:
            realPainter.drawPixmap(0, 0, context.pixmap());
            break;
        case ZLView::DEGREES90:
            realPainter.rotate(270);
            realPainter.drawPixmap(1 - height(), -1, context.pixmap());
            break;
        case ZLView::DEGREES180:
            realPainter.rotate(180);
            realPainter.drawPixmap(1 - width(), 1 - height(), context.pixmap());
            break;
        case ZLView::DEGREES270:
            realPainter.rotate(90);
            realPainter.drawPixmap(-1, 1 - width(), context.pixmap());
            break;
    }
    if (myHolder.hasBookmark())
    {
        drawBookmark(realPainter);
    }
}

void ZLQtViewWidget::Widget::drawBookmark(QPainter &painter)
{
    static QImage image(":/images/bookmark_flag.png");
    QPoint pt(rect().width()- image.width(), 0);
    painter.drawImage(pt, image);
}

void ZLQtViewWidget::Widget::mousePressEvent(QMouseEvent *event) {
    event->accept();
    last_pos_ = event->pos();

    // myHolder.view()->onStylusMove(x(event), y(event));
    // myHolder.view()->onStylusPress(x(event), y(event));
}

void ZLQtViewWidget::Widget::mouseReleaseEvent(QMouseEvent *event) {
    // myHolder.view()->onStylusRelease(x(event), y(event));
    stylusPan(event->pos(), last_pos_);
}

void ZLQtViewWidget::Widget::mouseMoveEvent(QMouseEvent *event) {
    event->accept();
    /*
    switch (event->buttons()) {
        case Qt::LeftButton:
            myHolder.view()->onStylusMovePressed(x(event), y(event));
            break;
        case Qt::NoButton:
            myHolder.view()->onStylusMove(x(event), y(event));
            break;
        default:
            break;
    }
    */
}

void ZLQtViewWidget::Widget::keyReleaseEvent(QKeyEvent *event)
{
    myHolder.view()->processKeyReleaseEvent(event->key());
}

int ZLQtViewWidget::Widget::x(const QMouseEvent *event) const {
    const int maxX = width() - 1;
    const int maxY = height() - 1;
    switch (myHolder.rotation()) {
        default:
            return std::min(std::max(event->x(), 0), maxX);
        case ZLView::DEGREES90:
            return maxY - std::min(std::max(event->y(), 0), maxY);
        case ZLView::DEGREES180:
            return maxX - std::min(std::max(event->x(), 0), maxX);
        case ZLView::DEGREES270:
            return std::min(std::max(event->y(), 0), maxY);
    }
}

int ZLQtViewWidget::Widget::y(const QMouseEvent *event) const {
    const int maxX = width() - 1;
    const int maxY = height() - 1;
    switch (myHolder.rotation()) {
        default:
            return std::min(std::max(event->y(), 0), maxY);
        case ZLView::DEGREES90:
            return std::min(std::max(event->x(), 0), maxX);
        case ZLView::DEGREES180:
            return maxY - std::min(std::max(event->y(), 0), maxY);
        case ZLView::DEGREES270:
            return maxX - std::min(std::max(event->x(), 0), maxX);
    }
}

void ZLQtViewWidget::enableTextSelection(bool enable)
{
    myApplication->enableTextSelection(enable);
}

bool ZLQtViewWidget::isTextSelectionEnabled()
{
    return myApplication->isTextSelectionEnabled();
}

void ZLQtViewWidget::Widget::stylusPan(const QPoint &now, const QPoint &old)
{
    int direction = sys::SystemConfig::direction(old, now);

    if (direction > 0)
    {
        myHolder.nextPage();
    }
    else if (direction < 0)
    {
        myHolder.prevPage();
    }
    else if (myHolder.isTextSelectionEnabled())
    {
        onyx::screen::ScreenProxy::Waveform w = onyx::screen::instance().defaultWaveform();
        onyx::screen::instance().setDefaultWaveform(onyx::screen::ScreenProxy::DW);
        bool ok = myHolder.view()->onStylusRelease(now.x(), now.y());
        onyx::screen::instance().setDefaultWaveform(w);
        if (ok)
        {
            myHolder.lookup();
        }
    }
    else
    {
        myHolder.view()->openInternalLink(now.x(), now.y());
    }
}

void ZLQtViewWidget::repaint()
{
    widget()->repaint();
}

void ZLQtViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
}

void ZLQtViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
}

void ZLQtViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
    if (status_bar_ && direction == ZLView::VERTICAL)
    {
        full_ = full;
        from_ = from;
        to_ = to;
        page_step_ = to_ - from_;
        updateProgress(full, from, to);
    }

    /*
    QScrollBar *bar =
    (direction == ZLView::VERTICAL) ?
    (myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar) :
    (myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar);
    bar->setMinimum(0);
    bar->setMaximum(full + from - to);
    bar->setValue(from);
    bar->setPageStep(to - from);
    */
}

QWidget *ZLQtViewWidget::widget() {
    return myFrame;
}

void ZLQtViewWidget::updateActions()
{
    // Encoding actions.
    std::string encoding = myApplication->EncodingOption.value();
    if (encoding == "GBK")
    {
        encoding = "GB18030";
    }
    encoding_actions_.generateActions(encoding);

    // Font family.
    QFont font = currentFont();
    font_family_actions_.generateActions(font.family(), true);

    // size
    std::vector<int> size;
    font_actions_.generateActions(font, size, font.pointSize());

    // Reading tools
    std::vector<ReadingToolsType> tools;
    tools.push_back(SEARCH_TOOL);
    tools.push_back(DICTIONARY_TOOL);
    reading_tool_actions_.generateActions(tools);

    // Reading tools of bookmark.
    tools.clear();
    tools.push_back(ADD_BOOKMARK);
    tools.push_back(DELETE_BOOKMARK);
    tools.push_back(SHOW_ALL_BOOKMARKS);
    reading_tool_actions_.generateActions(tools, true);
    bool has_bookmark = hasBookmark();
    reading_tool_actions_.action(ADD_BOOKMARK)->setEnabled(!has_bookmark);
    reading_tool_actions_.action(DELETE_BOOKMARK)->setEnabled(has_bookmark);

    // Reading tools of go to page.
    tools.clear();
    tools.push_back(GOTO_PAGE);
    reading_tool_actions_.generateActions(tools, true);
    int total = (full_ >> SHIFT);
    reading_tool_actions_.action(GOTO_PAGE)->setEnabled(total > 1);

    system_actions_.generateActions();
}

void ZLQtViewWidget::popupMenu()
{
    ui::PopupMenu menu(widget());
    updateActions();
    menu.addGroup(&font_family_actions_);
    menu.addGroup(&font_actions_);
    menu.addGroup(&encoding_actions_);
    menu.addGroup(&reading_tool_actions_);
    menu.setSystemAction(&system_actions_);
    if (menu.popup() != QDialog::Accepted)
    {
        return;
    }

    QAction * group = menu.selectedCategory();
    if (group == system_actions_.category())
    {
        SystemAction system = system_actions_.selected();
        if (system == RETURN_TO_LIBRARY)
        {
            quit();
        }
        else if (system == SCREEN_UPDATE_TYPE)
        {
            onyx::screen::instance().updateWidget(widget(), onyx::screen::ScreenProxy::GU);
            onyx::screen::instance().toggleWaveform();
        }
        else if (system == MUSIC)
        {
            // Start or show music player.
            onyx::screen::instance().flush(widget(), onyx::screen::ScreenProxy::GU);
            sys::SysStatus::instance().requestMusicPlayer(sys::START_PLAYER);
        }
        else if (system == ROTATE_SCREEN)
        {
            rotateScreen();
        }
        return;
    }

    if (group == encoding_actions_.category())
    {
        onSetEncoding(encoding_actions_.encoding());
    }
    else if (group == font_family_actions_.category())
    {
        changeFontFamily(font_family_actions_.selectedFont().toStdString());
    }
    else if (group == font_actions_.category())
    {
        changeFont(font_actions_.selectedFont());
    }
    else if (group == reading_tool_actions_.category())
    {
        if (reading_tool_actions_.selectedTool() == DICTIONARY_TOOL)
        {
            startDictLookup();
        }
        else if (reading_tool_actions_.selectedTool() == TEXT_TO_SPEECH)
        {
            // startTTS();
        }
        else if (reading_tool_actions_.selectedTool() == SEARCH_TOOL)
        {
            showSearchWidget();
        }
        else if (reading_tool_actions_.selectedTool() == GOTO_PAGE)
        {
            showGotoPageDialog();
        }
        else
        {
            processBookmarks(reading_tool_actions_);
        }
    }
}

bool ZLQtViewWidget::addBookmark()
{
    ZLTextView *ptr = static_cast<ZLTextView *>(view().get());
    std::string text;
    ptr->selectionModel().text(text, 100);
    QString title = QString::fromUtf8(text.c_str());

    vbf::Bookmark new_bookmark(title, from_);
    myApplication->addBookmark(new_bookmark);
    myApplication->refreshWindow();
    return true;
}

bool ZLQtViewWidget::removeBookmarks()
{
    myApplication->removeBookmarks(from_, to_);
    myApplication->refreshWindow();
    return true;
}

bool ZLQtViewWidget::clearBookmarks()
{
    myApplication->clearBookmarks();
    myApplication->refreshWindow();
    return true;
}

void ZLQtViewWidget::bookmarkModel(QStandardItemModel & model,
                                   QModelIndex & selected)
{
    model.setColumnCount(2);
    Bookmarks & all = myApplication->bookmarks();

    BookmarksIter begin = all.begin();
    BookmarksIter end   = all.end();
    int row = 0;
    for(BookmarksIter iter  = begin; iter != end; ++iter, ++row)
    {
        QStandardItem *title = new QStandardItem(iter->title());
        title->setData(iter->data());
        title->setEditable(false);
        model.setItem(row, 0, title);

        int pos = iter->data().toInt();
        QString str(tr("%1"));
        pos = (pos >> SHIFT);
        if (pos <= 0) ++pos;
        str = str.arg(pos);
        QStandardItem *page = new QStandardItem(str);
        page->setTextAlignment(Qt::AlignCenter);
        page->setEditable(false);
        model.setItem(row, 1, page);
    }
}

void ZLQtViewWidget::showAllBookmarks()
{
    QStandardItemModel model;
    QModelIndex selected = model.invisibleRootItem()->index();
    bookmarkModel(model, selected);

    TreeViewDialog bookmark_view(widget());
    bookmark_view.setModel(&model);
    QVector<int> percentages;
    percentages.push_back(80);
    percentages.push_back(20);
    bookmark_view.tree().setColumnWidth(percentages);
    int ret = bookmark_view.popup(QCoreApplication::tr("Bookmarks"));

    // Returned from the bookmark view.
    onyx::screen::instance().flush();

    if (ret != QDialog::Accepted)
    {
        onyx::screen::instance().updateWidget(0);
        return;
    }

    int pos = model.data(bookmark_view.selectedItem(), Qt::UserRole + 1).toInt();
    onScrollbarMoved(ZLView::VERTICAL, full_, pos, pos + page_step_);
}

bool ZLQtViewWidget::hasBookmark()
{
    return myApplication->hasBookmark(from_, to_);
}

void ZLQtViewWidget::processBookmarks(ReadingToolsActions & actions)
{
    if (actions.selectedTool() == ADD_BOOKMARK)
    {
        addBookmark();
    }
    else if (actions.selectedTool() == DELETE_BOOKMARK)
    {
        removeBookmarks();
    }
    else if (actions.selectedTool() == SHOW_ALL_BOOKMARKS)
    {
        showAllBookmarks();
    }
}


void ZLQtViewWidget::onProgressClicked(const int percentage,
                                       const int value)
{
    to_ = (value << SHIFT);
    if (to_ >= page_step_)
    {
        from_ = to_ - page_step_;
    }
    else
    {
        from_ = 0;
    }
    // John: ask framework to update status bar, so we don't need to process it here.
    // updateProgress(full_, from_, to_);
    onScrollbarMoved(ZLView::VERTICAL, full_, from_, to_);
}

void ZLQtViewWidget::onSetEncoding(std::string encoding)
{
    // Fallback to GBK if necessary.
    if (encoding == "GB18030")
    {
        encoding = "GBK";
    }
    myApplication->EncodingOption.setValue(encoding);
    myApplication->doAction("changeEncoding");
}

void ZLQtViewWidget::changeFontFamily(const std::string & family)
{
    ZLStringOption &familyOption = ZLTextStyleCollection::instance().baseStyle().FontFamilyOption;
    familyOption.setValue(family);
    myApplication->doAction("updateOptions");
}

QFont ZLQtViewWidget::currentFont()
{
    QFont font;
    ZLStringOption &familyOption = ZLTextStyleCollection::instance().baseStyle().FontFamilyOption;
    font.setFamily(familyOption.value().c_str());

    ZLIntegerRangeOption &sizeOption = ZLTextStyleCollection::instance().baseStyle().FontSizeOption;
    font.setPointSize(sizeOption.value());

    ZLBooleanOption &italicOption = ZLTextStyleCollection::instance().baseStyle().ItalicOption;
    font.setItalic(italicOption.value());

    ZLBooleanOption &boldOption = ZLTextStyleCollection::instance().baseStyle().BoldOption;
    font.setBold(boldOption.value());
    return font;
}

void ZLQtViewWidget::changeFont(QFont font)
{
    ZLIntegerRangeOption &sizeOption = ZLTextStyleCollection::instance().baseStyle().FontSizeOption;
    sizeOption.setValue(font.pointSize());

    ZLBooleanOption &italicOption = ZLTextStyleCollection::instance().baseStyle().ItalicOption;
    italicOption.setValue(font.italic());

    ZLBooleanOption &boldOption = ZLTextStyleCollection::instance().baseStyle().BoldOption;
    boldOption.setValue(font.bold());
    myApplication->doAction("updateOptions");
}

void ZLQtViewWidget::rotateScreen()
{
    sys::SysStatus::instance().rotateScreen();
}

void ZLQtViewWidget::quit()
{
    myApplication->doAction("quit");
}

void ZLQtViewWidget::loadConf()
{
    conf_stored_ = false;
    Configuration & conf = myApplication->conf();
    onyx::screen::ScreenProxy::Waveform type = static_cast<onyx::screen::ScreenProxy::Waveform>(conf.options[CONFIG_FLASH_TYPE].toInt());
    if (type == onyx::screen::ScreenProxy::GU)
    {
        onyx::screen::instance().setDefaultWaveform(type);
    }
    else
    {
        onyx::screen::instance().setDefaultWaveform(onyx::screen::ScreenProxy::GC);
    }
}

void ZLQtViewWidget::saveConf()
{
    if (conf_stored_)
    {
        return;
    }

    conf_stored_ = true;
    Configuration & conf = myApplication->conf();
    QString progress("%1 / %2");
    int pos = (to_ >> SHIFT);
    if (pos <= 0)
    {
        pos = 1;
    }
    int total = (full_ >> SHIFT);
    if (total <= 0)
    {
        total = 1;
    }
    conf.info.mutable_progress() = progress.arg(pos).arg(total);
    conf.options[CONFIG_FLASH_TYPE] = onyx::screen::instance().defaultWaveform();
}

void ZLQtViewWidget::closeDocument()
{
    saveConf();
}

void ZLQtViewWidget::fastRefreshWindow(bool full_update)
{
    if (!full_update)
    {
        onyx::screen::ScreenProxy::Waveform w = onyx::screen::instance().defaultWaveform();
        onyx::screen::instance().setDefaultWaveform(onyx::screen::ScreenProxy::DW);
        myApplication->refreshWindow(false);
        onyx::screen::instance().setDefaultWaveform(w);
    }
    else
    {
        myApplication->refreshWindow(false);
    }
}

void ZLQtViewWidget::lookupAndUpdate()
{
    fastRefreshWindow(false);
    if (adjustDictWidget())
    {
        fastRefreshWindow(true);
    }
}

void ZLQtViewWidget::showGotoPageDialog()
{
    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
    status_bar_->onMessageAreaClicked();
}

void ZLQtViewWidget::processKeyReleaseEvent(int key)
{
    ZLTextView *ptr = static_cast<ZLTextView *>(view().get());
    if (myApplication->isTextSelectionEnabled())
    {
        switch (key)
        {
        case Qt::Key_Right:
            if (ptr->selectionModel().selectNextWord())
            {
                lookupAndUpdate();
            }
            break;
        case Qt::Key_Left:
            if (ptr->selectionModel().selectPrevWord())
            {
                lookupAndUpdate();
            }
            break;
        case Qt::Key_Up:
            if (ptr->selectionModel().selectPrevLineWord())
            {
                lookupAndUpdate();
            }
            break;
        case Qt::Key_Down:
            if (ptr->selectionModel().selectNextLineWord())
            {
                lookupAndUpdate();
            }
            break;
        case Qt::Key_Return:
            lookup();
            break;
        case Qt::Key_Escape:
            stopDictLookup();
            break;
        }
    }
    else
    {
        if (key == Qt::Key_Return)
        {
            showGotoPageDialog();
        }
    }
}

void ZLQtViewWidget::updateProgress(size_t full, size_t from, size_t to)
{
    int current = (to >> SHIFT);
    if (current <= 0)
    {
        ++current;
    }
    int total = (full >> SHIFT);
    if (total <= 0)
    {
        ++total;
    }
    status_bar_->setProgress(current, total);
}

bool ZLQtViewWidget::isWidgetVisible(QWidget * wnd)
{
    if (wnd)
    {
        return wnd->isVisible();
    }
    return false;
}

void ZLQtViewWidget::hideHelperWidget(QWidget * wnd)
{
    if (wnd)
    {
        wnd->hide();
    }
}

void ZLQtViewWidget::startDictLookup()
{
    enableTextSelection(true);

    if (!dicts_)
    {
        dicts_.reset(new DictionaryManager);
    }

    if (!tts_engine_)
    {
        // tts_engine_.reset(new TTS(QLocale::system()));
    }

    if (!dict_widget_)
    {
        dict_widget_.reset(new DictWidget(widget(), *dicts_, tts_engine_.get()));
        connect(dict_widget_.get(), SIGNAL(keyReleaseSignal(int)), this, SLOT(processKeyReleaseEvent(int)));
        connect(dict_widget_.get(), SIGNAL(closeClicked()), this, SLOT(onDictClosed()));
    }

    hideHelperWidget(tts_widget_.get());
    hideHelperWidget(search_widget_.get());

    // When dictionary widget is not visible, it's necessary to update the text view.
    dict_widget_->lookup(selected_text_);
    dict_widget_->ensureVisible(selected_rect_, true);
}

void ZLQtViewWidget::stopDictLookup()
{
    onDictClosed();
    hideHelperWidget(dict_widget_.get());
    dict_widget_.reset(0);
}

void ZLQtViewWidget::showSearchWidget()
{
    if (!search_widget_)
    {
        search_widget_.reset(new SearchWidget(widget(), search_context_));
        connect(search_widget_.get(), SIGNAL(search(BaseSearchContext &)),
            this, SLOT(onSearch(BaseSearchContext &)));
        connect(search_widget_.get(), SIGNAL(closeClicked()), this, SLOT(onSearchClosed()));
    }

    search_context_.userData() = BEFORE_SEARCH;
    hideHelperWidget(dict_widget_.get());
    hideHelperWidget(tts_widget_.get());
    search_widget_->ensureVisible();
}

bool ZLQtViewWidget::updateSearchCriteria()
{
    return true;
}

void ZLQtViewWidget::onSearch(BaseSearchContext& context)
{
    if (search_context_.userData() <= BEFORE_SEARCH)
    {
        myApplication->SearchPatternOption.setValue(context.pattern().toUtf8().constData());
        myApplication->SearchForwardOption.setValue(context.forward());
        myApplication->doAction("search");
        search_context_.userData() = IN_SEARCHING;

        // TODO
        updateSearchWidget();
    }
    else
    {
        if (context.forward())
        {
            if (updateSearchWidget())
            {
                myApplication->doAction("findNext");
            }
        }
        else
        {
            if (updateSearchWidget())
            {
                myApplication->doAction("findPrevious");
            }
        }
    }
}

/// Return true if we can continue searching.
bool ZLQtViewWidget::updateSearchWidget()
{
    if (search_context_.forward())
    {
        if (!myApplication->action("findNext")->isEnabled())
        {
            search_widget_->noMoreMatches();
            return false;
        }
    }
    else
    {
        if (!myApplication->action("findPrevious")->isEnabled())
        {
            search_widget_->noMoreMatches();
            return false;
        }
    }
    return true;
}

void ZLQtViewWidget::lookup()
{
    ZLTextView *ptr = static_cast<ZLTextView *>(view().get());
    selected_text_ = QString::fromUtf8(ptr->selectionModel().text().c_str());
    adjustDictWidget();
    dict_widget_->lookup(selected_text_);
}

bool ZLQtViewWidget::adjustDictWidget()
{
    ZLTextView *ptr = static_cast<ZLTextView *>(view().get());
    const ZLTextElementArea & area = ptr->selectionModel().wordArea();
    if (area.XEnd == area.XStart || area.YStart == area.YEnd)
    {
        return false;
    }
    selected_rect_.setCoords(area.XStart, area.YStart, area.XEnd, area.YEnd);
    return dict_widget_->ensureVisible(selected_rect_);
}

void ZLQtViewWidget::onDictClosed()
{
    myApplication->doAction("clearSelection");
    enableTextSelection(false);
}

void ZLQtViewWidget::onSearchClosed()
{
    myApplication->doAction("clearSearchResult");
}

void ZLQtViewWidget::nextPage()
{
    myApplication->doAction("largeScrollForward");
}

void ZLQtViewWidget::prevPage()
{
    myApplication->doAction("largeScrollBackward");
}

void ZLQtViewWidget::onSdCardChanged(bool inserted)
{
    QString path = QString::fromLocal8Bit(myApplication->DocumentPathOption.value().c_str());
    if (!inserted && path.startsWith(SDMMC_ROOT))
    {
        quit();
    }
}

void ZLQtViewWidget::onWakeup()
{
    font_family_actions_.reloadExternalFonts();
}

/// Handle mount tree event including internal flash and sd card.
void ZLQtViewWidget::handleMountTreeEvent(bool inserted, const QString &mount_point)
{
    QString path = QString::fromLocal8Bit(myApplication->DocumentPathOption.value().c_str());
    if (!inserted && path.startsWith(mount_point))
    {
        quit();
    }
}

void ZLQtViewWidget::onAboutToShutdown()
{
    quit();
}

void ZLQtViewWidget::onMusicPlayerStateChanged(int state)
{
    if (state == sys::HIDE_PLAYER || state == sys::STOP_PLAYER)
    {
        onyx::screen::instance().flush();
        myApplication->refreshWindow();
    }
}

