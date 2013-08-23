#ifndef EXPLORER_URL_WIDGET_H_
#define EXPLORER_URL_WIDGET_H_

#include "onyx/ui/ui.h"
#include "branch_node.h"

using namespace ui;
using namespace explorer::model;

namespace explorer
{

namespace view
{

/// Rename dialog to enable user to rename files or folders.
class UrlDialog : public OnyxDialog
{
    Q_OBJECT

public:
    UrlDialog(QWidget *parent);
    ~UrlDialog(void);

public:
    QString popup(const QString & name);

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void onConfirmClicked();
    void onCloseClicked();
    void onTextChanged(const QString&);
    void forceUpdate();
    void onTimeout();

private:
    void createLayout();
    void updateChildrenWidgets(bool searching);
    void updateTitle(const QString &message = QString());

private:
    QHBoxLayout  hbox_;

    OnyxLineEdit text_edit_;    ///< Input edit.
    OnyxPushButton   confirm_button_;   ///< Start to search.
    OnyxPushButton   clear_button_;   ///< Clear the text.

    KeyBoard      keyboard_;     ///< Keyboard.

    UpdateState update_state_;
};

}   // namespace view

}   // namespace explorer


#endif  // EXPLORER_URL_WIDGET_H_
