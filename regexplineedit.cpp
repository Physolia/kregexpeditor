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
#include "regexplineedit.h"

RegExpLineEdit::RegExpLineEdit( QWidget* parent, const char* name )
    :QLineEdit( parent, name ), _editable( true )
{
    installEventFilter( this );
}

void RegExpLineEdit::setEditable( bool b )
{
    _editable = b;
}

bool RegExpLineEdit::eventFilter( QObject*, QEvent* ev )
{
    if ( !_editable && ( ev->type() >= QEvent::MouseButtonPress && ev->type() <= QEvent::FocusOut ||
                         ev->type() == QEvent::ContextMenu ) )
        return true;
    else
        return false;
}
