#ifndef COMPAT_H
#define COMPAT_H
#include <qobject.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qframe.h>

QString i18n( const QString& a);
QString i18n( const QString& a, const QString& b);
#define isatty(x) 0

#define KTextBrowser QTextBrowser
#define KListBox QListBox
#define KFileDialog QFileDialog

class KDialogBase :public QDialog
{
    Q_OBJECT

public:
    enum ButtonCode {Ok = 1, Cancel, Help};
    enum DialogType { Plain };

    KDialogBase ( int dialogFace, const QString &caption, int buttonMask,
                  ButtonCode defaultButton,
                  QWidget *parent=0, const char *name=0, bool modal=true );

    KDialogBase( QWidget* parent, const char* name = 0, bool modal = true,
                 const QString& caption = QString::null,
                 int buttonMask = 0 );

    void init( int buttonMask, ButtonCode /*defaultButton*/, const QString& caption );
    void setMainWidget( QWidget* top );
    QFrame* plainPage();
    void setHelp( const QString&, const QString& ) {}

public slots:
    void slotOk();
    void slotCancel();

signals:
    void okClicked();
    void cancelClicked();
    void finished();
    void helpClicked();

private:
    QVBoxLayout* _layout;
};

class KMessageBox :public QMessageBox
{
    Q_OBJECT
public:
    enum ButtonCode { Ok = 1, Cancel = 2, Yes = 3, No = 4, Continue = 5 };
    static int  warningYesNo (QWidget *parent, const QString &text,
                              const QString &caption = QString::null );
    static int information( QWidget* parent, const QString& text, const QString& caption = QString::null,
                            const QString& /*dontShowAgainName*/ = QString::null );
    static int sorry( QWidget* parent, const QString& text, const QString& caption = QString::null );
};

#endif /* COMPAT_H */
