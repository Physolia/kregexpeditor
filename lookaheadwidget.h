/*
 *  Copyright (c) 2002-2003 Jesper K. Pedersen <blackie@kde.org>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/
#ifndef __lookaheadwidget
#define __lookaheadwidget

#include "singlecontainerwidget.h"
class LookAheadRegExp;

class LookAheadWidget :public SingleContainerWidget
{
  Q_OBJECT
public:
  LookAheadWidget( RegExpEditorWindow* editorWindow, RegExpType tp, QWidget* parent, const char* name = 0 );
  LookAheadWidget( LookAheadRegExp* regexp, RegExpEditorWindow* editorWindow, RegExpType tp,
                   QWidget* parent, const char* name = 0);

 	virtual RegExp* regExp() const;
 	virtual RegExpType type() const { return _tp; }
  virtual QSize sizeHint() const;

protected:
  void init();
  virtual void paintEvent( QPaintEvent *e );


private:
  RegExpType _tp;
  QString _text;

  mutable QSize _textSize;
  mutable QSize _childSize;
};

#endif // __lookaheadwidget
