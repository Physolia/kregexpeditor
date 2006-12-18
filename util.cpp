/*
 *  Copyright (c) 2002-2004 Jesper K. Pedersen <blackie@kde.org>
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

#include "util.h"
#include <kicon.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

QPixmap Util::getKRegExpEditorIcon( const QString& name )
{
#ifdef QT_ONLY
    QPixmap pix;
    pix.convertFromImage( qembed_findImage(name) );
    return pix;
#else
  return BarIcon(KStandardDirs::locate("data", QString::fromLatin1("kregexpeditor/pics/") +name ) );
#endif
}

QPixmap Util::getSystemIcon( const QString& name )
{
#ifdef QT_ONLY
    QPixmap pix;
    pix.convertFromImage( qembed_findImage( name ) );
    return pix;
#else
  return BarIcon( name );
#endif

}

QIcon Util::getSystemIconSet( const QString& name )
{
#ifdef QT_ONLY
    QPixmap pix;
    pix.convertFromImage( qembed_findImage( name ) );
    return QIcon( pix );
#else
  return KIcon( name );
#endif

}
