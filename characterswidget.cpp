#include "characterswidget.h"
#include "textrangeregexp.h"
#include "altnregexp.h"
#include <qpainter.h>
#include <qgrid.h>
#include <iostream>
#include <qhgroupbox.h>
#include <kdialogbase.h>
#include "charselector.h"
#include "myfontmetrics.h"
#include <qcursor.h>
#include <qapplication.h>

CharactersWidget::CharactersWidget(RegExpEditorWindow* editorWindow, QWidget *parent,
                                   const char *name)
    : RegExpWidget(editorWindow, parent, name), _configWindow(0)
{
    _regexp = new TextRangeRegExp( false /* not used */);
}

CharactersWidget::CharactersWidget( TextRangeRegExp* regexp, RegExpEditorWindow* editorWindow,
                                    QWidget* parent, const char* name )
    : RegExpWidget( editorWindow, parent, name ), _configWindow(0)
{
    _regexp = dynamic_cast<TextRangeRegExp*>( regexp->clone() );
    Q_ASSERT( _regexp );
}

CharactersWidget::~CharactersWidget()
{
    delete _regexp;
}


QSize CharactersWidget::sizeHint() const
{
    QFontMetrics metrics = fontMetrics();
    _textSize = HackCalculateFontSize(metrics, title());
    //  _textSize = metrics.size(0, title());
  
    QSize headerSize = QSize(_textSize.width() + 4 * bdSize,
                             _textSize.height());
  
    _contentSize = HackCalculateFontSize(metrics,text());
    //  _contentSize = metrics.size(0, text());
  
    return QSize(QMAX(headerSize.width(), bdSize + _contentSize.width() + bdSize+ 2*pw),
                 headerSize.height() + bdSize + _contentSize.height() + bdSize + 2*pw);
}

void CharactersWidget::paintEvent(QPaintEvent *e) 
{
    QSize mySize = sizeHint();

    QPainter painter(this);
    drawPossibleSelection( painter, mySize );
  
    int center = _textSize.height()/2;
    int offset = 0;
    
    // draw the horizontal line and the label
    painter.drawLine(pw,center, bdSize,center);
    offset += pw + 2*bdSize;
  
    painter.drawText(offset, 0, _textSize.width(), _textSize.height(), 0, title());
    offset += _textSize.width() + bdSize;
    
    painter.drawLine(offset, center, mySize.width(), center);

    // Draw the rest of the lines
    int y = mySize.width() - pw;
    int x = mySize.height() -pw;
    painter.drawLine(0,center,0,mySize.height());
    painter.drawLine(y,center,y,mySize.height());
    painter.drawLine(0,x,y,x);
  
    // Draw the text
    painter.drawText(bdSize, bdSize+_textSize.height(), _contentSize.width(), 
                     _contentSize.height(), 0, text());
 
    RegExpWidget::paintEvent(e);
}

RegExp* CharactersWidget::regExp() const
{
	RegExp* r = _regexp->clone();	
    r->setSelected( isSelected() );
    return r;
}

QString CharactersWidget::text() const
{
    QString res = QString::fromLatin1("");
  
    if (_regexp->wordChar())
        res += i18n("- A word character\n");
  
    if (_regexp->nonWordChar())
        res += i18n("- A non-word character\n");
  
    if (_regexp->digit())
        res += i18n("- A digit character\n");

    if (_regexp->nonDigit())
        res += i18n("- A non-digit character\n");

    if ( _regexp->space() )
        res += i18n("- A space character\n");
  
    if ( _regexp->nonSpace() )
        res += i18n("- A non-space character\n");
  
    // Single characters
    QStringList chars = _regexp->chars();
    if ( !chars.isEmpty() ) {
        QString str = chars.join( QString::fromLocal8Bit(", ") );
        res += QString::fromLocal8Bit("- ") + str + QString::fromLocal8Bit("\n");
    }

    // Ranges characters
    QPtrList<StringPair> range = _regexp->range();
    for ( QPtrListIterator<StringPair> it( range ); *it; ++it ) {
        StringPair* elm = dynamic_cast<StringPair*>(*it);
        QString fromText = elm->first();
        QString toText = elm->second();
    
        res += QString::fromLocal8Bit("- ") + i18n("from ") + fromText + i18n(" to ") + toText + QString::fromLocal8Bit("\n");
    }
    return res.left(res.length()-1);
}


QString CharactersWidget::title() const
{
    if (_regexp->negate()) 
        return i18n("Any character except");
    else
        return i18n("One of the following characters");
}


RegExpWidget* CharactersWidget::findWidgetToEdit( QPoint globalPos )
{
    if ( QRect(mapToGlobal(QPoint(0,0)), size()).contains( globalPos ) )
        return this;
    else
        return 0;
}

int CharactersWidget::edit() 
{
    if ( _configWindow == 0 ) {
        QApplication::setOverrideCursor( WaitCursor );
        _configWindow = new CharacterEdits( _regexp, this, "CharactersWidget::_configWindow" );
        QApplication::restoreOverrideCursor();
    }
  
    _configWindow->move(QCursor::pos() - QPoint(_configWindow->sizeHint().width()/2,
                                                _configWindow->sizeHint().height()/2));
    int ret = _configWindow->exec();
    if ( ret == QDialog::Accepted ) {
        _editorWindow->updateContent( 0 );
        update();
    }
    return ret;
}

void CharacterEdits::addCharacter( QString txt )
{
    KMultiFormListBoxEntryList list = _single->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it(list); *it; ++it ) {
        SingleEntry* entry = dynamic_cast<SingleEntry*>( *it );
        if ( entry->isEmpty() ) {
            entry->setText( txt );
            return;
        }
    }
  
    SingleEntry* entry = new SingleEntry( _single );
    entry->setText( txt );
    _single->append( entry );
}

void CharacterEdits::addRange( QString from, QString to )
{
    KMultiFormListBoxEntryList list = _range->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it(list); *it; ++it ) {
        RangeEntry* entry = dynamic_cast<RangeEntry*>( *it );
        if ( entry->isEmpty() ) {
            entry->setFrom( from );
            entry->setTo( to );
            return;
        }
    }
  
    RangeEntry* entry = new RangeEntry( _range );
    entry->setFrom( from );
    entry->setTo( to );
    _range->append( entry );  
}

int CharacterEdits::exec()
{
    negate->setChecked( _regexp->negate() );
    digit->setChecked( _regexp->digit() );
    nonDigit->setChecked( _regexp->nonDigit() );
    space->setChecked( _regexp->space() );
    nonSpace->setChecked( _regexp->nonSpace() );
    wordChar->setChecked( _regexp->wordChar() );
    nonWordChar->setChecked( _regexp->nonWordChar() );

    // Characters

    KMultiFormListBoxEntryList list1 = _single->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it(list1); *it; ++it ) {
        SingleEntry* entry = dynamic_cast<SingleEntry*>( *it );
        entry->setText( QString::fromLocal8Bit("") );
    }
    QStringList list2 = _regexp->chars();
    for ( QStringList::Iterator it( list2.begin() ); ! (*it).isNull(); ++it ) {
        addCharacter( *it );
    }

    // Ranges
    KMultiFormListBoxEntryList list3 = _range->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it(list3); *it; ++it ) {
        RangeEntry* entry = dynamic_cast<RangeEntry*>( *it );
        entry->setFrom( QString::fromLocal8Bit("") );
        entry->setTo( QString::fromLocal8Bit("") );
    }
  
    QPtrList<StringPair> ranges = _regexp->range();
    for ( QPtrListIterator<StringPair> it(ranges); *it; ++it ) {
        QString from = (*it)->first();
        QString to = (*it)->second();
        addRange(from,to);
    }

    return KDialogBase::exec();
}


CharacterEdits::CharacterEdits(TextRangeRegExp* regexp, QWidget *parent, const char *name)
  : KDialogBase( parent, name == 0 ? "CharacterEdits" : name, true, 
                 i18n("Specify Characters"),
                 KDialogBase::Ok | KDialogBase::Cancel),
    _regexp( regexp )
{
    QWidget* top = new QWidget( this );
    QVBoxLayout *topLayout = new QVBoxLayout(top, 6);
    setMainWidget( top );
    
    negate = new QCheckBox(i18n("Do not match the characters specified here"),
                           top);
    topLayout->addWidget( negate );
    
    
    // The predefined box
    QHGroupBox* predefined = new QHGroupBox(i18n("Predefined Character Ranges"),top);
    topLayout->addWidget(predefined);
    QGrid* grid = new QGrid(3, predefined );
    
    wordChar = new QCheckBox(i18n("A word character"),grid);
    digit = new QCheckBox(i18n("A digit character"),grid);
    space = new QCheckBox(i18n("A space character"), grid);
    
    nonWordChar = new QCheckBox(i18n("A non-word character"),grid);
    nonDigit = new QCheckBox(i18n("A non-digit character"),grid);
    nonSpace = new QCheckBox(i18n("A non-space character"), grid);
    
    // Single characters
    QVGroupBox* singleBox = new QVGroupBox(i18n("Single Characters"), top );
    topLayout->addWidget( singleBox );
    _single = new KMultiFormListBox(new SingleFactory(), KMultiFormListBox::MultiVisible, 
                                    singleBox);
    _single->addElement(); _single->addElement(); _single->addElement(); 
    
    QWidget* moreW = new QWidget( singleBox );
    QHBoxLayout* moreLay = new QHBoxLayout( moreW );
    QPushButton* more = new QPushButton( i18n("More Entries"), moreW );
    moreLay->addWidget( more );
    moreLay->addStretch( 1 );
    
    connect(more,SIGNAL(clicked()), _single, SLOT(addElement()));  
    
    // Ranges
    QVGroupBox* rangeBox = new QVGroupBox(i18n("Character Ranges"), top );
    topLayout->addWidget( rangeBox );
    
    _range = new KMultiFormListBox(new RangeFactory(), KMultiFormListBox::MultiVisible, 
                                   rangeBox);
    _range->addElement(); _range->addElement(); _range->addElement(); 
    
    moreW = new QWidget( rangeBox );
    moreLay = new QHBoxLayout( moreW );
    more = new QPushButton( i18n("More Entries"), moreW );
    moreLay->addWidget( more );
    moreLay->addStretch( 1 );
    connect(more,SIGNAL(clicked()), _range, SLOT(addElement()));  
    // Buttons
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOK()));
}

void CharacterEdits::slotOK()
{
    _regexp->setNegate( negate->isChecked() );

    _regexp->setWordChar( wordChar->isChecked() );
    _regexp->setNonWordChar( nonWordChar->isChecked() );

    _regexp->setDigit( digit->isChecked() );
    _regexp->setNonDigit( nonDigit->isChecked() );

    _regexp->setSpace( space->isChecked() );
    _regexp->setNonSpace( nonSpace->isChecked() );

	// single characters
    _regexp->clearChars();
    KMultiFormListBoxEntryList list = _single->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it( list ); *it; ++it ) {
        SingleEntry* entry = dynamic_cast<SingleEntry*>(*it);
        if ( !entry->isEmpty() ) {
            _regexp->addCharacter( entry->text() );
        }
    }

    // Ranges
    _regexp->clearRange();
    list = _range->elements();
    for ( QPtrListIterator<KMultiFormListBoxEntry> it( list ); *it; ++it ) {
        RangeEntry* entry = dynamic_cast<RangeEntry*>(*it);
        if ( !entry->isEmpty() ) {
            _regexp->addRange( entry->fromText(), entry->toText() );
        }
    }
}


SingleEntry::SingleEntry(QWidget* parent, const char* name )
    :KMultiFormListBoxEntry( parent, name )
{
    QHBoxLayout* layout = new QHBoxLayout( this, 3, 6 );
    _selector = new CharSelector( this );
    layout->addWidget( _selector );
    layout->addStretch(1);
}

QString SingleEntry::text() const 
{
    return _selector->text(); 
}

void SingleEntry::setText( QString text ) 
{
    _selector->setText( text ); 
}

bool SingleEntry::isEmpty() const 
{
    return _selector->isEmpty();
}


RangeEntry::RangeEntry(QWidget* parent, const char* name )
    :KMultiFormListBoxEntry( parent, name )
{
    QHBoxLayout* layout = new QHBoxLayout( this, 3, 6 );

    QLabel* label = new QLabel(i18n("From:"), this );
    _from = new CharSelector( this );
    layout->addWidget( label );
    layout->addWidget( _from );
  
    layout->addStretch(1);

    label = new QLabel(i18n("To:"), this );
    _to = new CharSelector( this );
    layout->addWidget( label );
    layout->addWidget( _to );
}

QString RangeEntry::fromText() const
{
    return _from->text();
}

QString RangeEntry::toText() const
{
    return _to->text();
}

void RangeEntry::setFrom( QString text ) 
{
    _from->setText( text ); 
}

void RangeEntry::setTo( QString text ) 
{
    _to->setText( text ); 
}

bool RangeEntry::isEmpty() const 
{
    return _from->isEmpty() || _to->isEmpty();
}

#include "characterswidget.moc"
