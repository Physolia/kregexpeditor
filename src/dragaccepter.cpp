/*
 *  Copyright (c) 2002-2003 Jesper K. Pedersen <Qt::blackie@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include "dragaccepter.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>

#include "concwidget.h"

DragAccepter::DragAccepter(RegExpEditorWindow *editorWindow, RegExpWidget *parent)
    : RegExpWidget(editorWindow, parent)
    , _drawLine(false)
{
    setAcceptDrops(true);
}

QSize DragAccepter::sizeHint() const
{
    return QSize(10, 10);
}

void DragAccepter::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    drawPossibleSelection(painter, size());

    if (_drawLine) {
        painter.drawLine(0, height() / 2, width(), height() / 2);
    }

    RegExpWidget::paintEvent(e);
}

void DragAccepter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        _editorWindow->showRMBMenu(_editorWindow->hasSelection());
    } else {
        RegExpWidget::mousePressEvent(event);
    }
}

void DragAccepter::mouseReleaseEvent(QMouseEvent *event)
{
    if (_editorWindow->isPasteing() && event->button() == Qt::LeftButton) {
        RegExp *regexp = _editorWindow->pasteData();

        RegExpWidget *newElm = WidgetFactory::createWidget(regexp, _editorWindow, nullptr);
        if (newElm) {
            ConcWidget *elm;
            if (!(elm = dynamic_cast<ConcWidget *>(newElm))) {
                elm = new ConcWidget(_editorWindow, newElm, nullptr);
            }

            Q_ASSERT(elm);

            RegExpWidget *w = dynamic_cast<RegExpWidget *>(parent());
            if (w) {
                w->addNewConcChild(this, elm);
            }
            _editorWindow->updateContent(this);
            _editorWindow->clearSelection(true);
        }
    } else if (_editorWindow->isInserting() && event->button() == Qt::LeftButton) {
        if (WidgetFactory::isContainer(_editorWindow->insertType())
            && _editorWindow->pointSelected(mapToGlobal(event->pos()))) {
            RegExpWidget::mouseReleaseEvent(event);
        } else {
            RegExpWidget *child = WidgetFactory::createWidget(_editorWindow,
                                                              dynamic_cast<QWidget *>(parent()),
                                                              _editorWindow->insertType());
            if (child) {
                RegExpWidget *w = dynamic_cast<RegExpWidget *>(parent());
                if (w) {
                    w->addNewChild(this, child);
                }
                _editorWindow->updateContent(child);
                child->setFocus();
                _editorWindow->clearSelection(true);
            }
        }
    }
    _editorWindow->slotEndActions();
}

void DragAccepter::dragEnterEvent(QDragEnterEvent *event)
{
    bool selfDrag = (event->source() && _isSelected);
    if (!selfDrag && event->mimeData()->hasFormat(QStringLiteral("KRegExpEditor/widgetdrag"))) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void DragAccepter::dropEvent(QDropEvent *event)
{
    // The widget will be reparent afterward or part of it will, so no need to give
    // it a parent here.
    QString name = QString::fromLatin1(event->mimeData()->data(QStringLiteral("KRegExpEditor/widgetdrag")));
    RegExp *regexp = WidgetFactory::createRegExp(name);
    RegExpWidget *newElm = WidgetFactory::createWidget(regexp, _editorWindow, nullptr);
    ConcWidget *elm;
    if (!(elm = dynamic_cast<ConcWidget *>(newElm))) {
        elm = new ConcWidget(_editorWindow, newElm, nullptr);
    }

    Q_ASSERT(elm);

    RegExpWidget *rew = dynamic_cast<RegExpWidget *>(parent());
    if (rew) {
        rew->addNewConcChild(this, elm);
    }

    QWidget *w = dynamic_cast<QWidget *>(parent());
    if (w) {
        w->update();
    }
    _editorWindow->updateContent(this);

    bool selfDrag = (event->source());
    if (!selfDrag) {
        _editorWindow->clearSelection(true);
    } else {
        // selection should not be cleared here, since we might want to delete it.
    }
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

RegExp *DragAccepter::regExp() const
{
    return nullptr;
}
