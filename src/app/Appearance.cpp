#include "Appearance.h"

#include "../app/AppSettings.h"
#include "../app/HelpSystem.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriTheme.h"
#include "helpers/OriTools.h"
#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "widgets/OriFlatToolBar.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QStyle>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QUrl>
#include <QXmlStreamReader>

namespace Z {
namespace Gui {

QString reportStyleSheet()
{
    static auto styleSheet = QString::fromLatin1(
        ".value {%1} "
        ".param {%2}"
        ".elem_label {%3}"
        ".elem_link {text-decoration:none}"
    ).arg(
        html(ValueFont()),
        html(ParamLabelFont()),
        html(ElemLabelFont())
    );
    return styleSheet;
}

QString fontToHtmlStyles(const QFont& font)
{
    QStringList styles;
    styles << QStringLiteral("font:");
    if (font.bold())
        styles << QStringLiteral("bold");
    else if (font.italic())
        styles << QStringLiteral("italic");
    else
        styles << QStringLiteral("normal");
    styles << QChar(' ');
    styles << QString::number(font.pointSize()) % QStringLiteral("pt");
    styles << QChar(' ');
    styles << QChar('"') << font.family() << QChar('"');
    return styles.join(QString());
}

QFont ValueFont::get() const
{
    QFont f = QApplication::font();
    if (_bold)
        f.setBold(true);
    if (_readOnly)
        f.setItalic(true);
#if defined(Q_OS_MAC)
    f.setPointSize(14);
#else
    if (f.pointSize() < 10)
        f.setPointSize(10);
#endif
    return f;
}

QFont CodeEditorFont::get() const
{
    QFont f = QApplication::font();
#if defined(Q_OS_WIN)
    f.setFamilies({"Cascadia Code", "Source Code Pro", "Consolas", "Lucida Console", "Courier New"});
    f.setPointSize(10);
#elif defined(Q_OS_MAC)
    f.setFamily("Monaco");
    f.setPointSize(13);
#else
    f.setFamily("monospace");
    f.setPointSize(11);
#endif
    return f;
}

QFont ElemLabelFont::get() const
{
    QFont f = QApplication::font();
    f.setBold(true);
#if defined(Q_OS_WIN)
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setFamily(QStringLiteral("serif"));
#endif
    int sizePt =
#if defined(Q_OS_WIN)
        11
#elif defined(Q_OS_MAC)
        16
#else
        10
#endif
    ;
    f.setPointSize(sizePt);
    return f;
}

QFont FormulaFont::get() const
{
    auto f = QApplication::font();
    f.setItalic(true);
#if defined(Q_OS_WIN)
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setFamily(QStringLiteral("serif"));
#endif
    int sizePt =
#if defined(Q_OS_WIN)
        11
#elif defined(Q_OS_MAC)
        16
#else
        10
#endif
    ;
    f.setPointSize(sizePt);
    return f;
}

QLabel* makeHeaderLabel(const QString& text)
{
    auto label = new QLabel(text);
    auto f = label->font();
    f.setPointSize(f.pointSize() + 2);
    f.setBold(true);
    label->setFont(f);
    label->setStyleSheet(QString("color:%1").arg(selectionColor().name()));
    return label;
}

QTabWidget* makeBorderlessTabs()
{
    auto tabs = new QTabWidget;
    tabs->setProperty("look", "borderless");
    return tabs;
}

QToolBar* makeToolbar(std::initializer_list<QObject *> items, const QString &helpTopic)
{
    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(AppSettings::instance().toolbarIconSize());
    Ori::Gui::populate(toolbar, items);
    if (!helpTopic.isEmpty())
    {
        toolbar->addSeparator();
        toolbar->addAction(HelpSystem::makeHelpAction(toolbar, helpTopic));
    }
    return toolbar;
}

void setFocusedBackground(QWidget *w, bool focused)
{
    QPalette p;
    if (focused)
        p.setColor(QPalette::Window, Z::Gui::lightSelectionColor());
    w->setAutoFillBackground(focused);
    w->setPalette(p);
}

void applyTextBrowserStyleSheet(QTextBrowser* browser, const QString& cssResourcePath)
{
    QFile f(cssResourcePath);
    if (!f.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open resource file" << f.fileName() << f.errorString();
        return;
    }
    browser->document()->setDefaultStyleSheet(QString::fromUtf8(f.readAll()));
    if (!AppSettings::instance().isDevMode) return;

    browser->connect(browser, &QTextBrowser::anchorClicked, browser, [cssResourcePath, browser](const QUrl& url){
        if (url.scheme() != "do" or url.host() != "edit-css") return;

        auto parts = cssResourcePath.split('/');
        if (parts.size() != 3)
        {
            qWarning() << "Unsupported resource path format" << cssResourcePath
                       << "it should be of pattern ':/prefix/alias'";
            return;
        }
        QString prefix = '/' + parts[1];
        QString alias = parts[2];
        QString styleFile;
        // TODO: adjust for macOS
        QFile f(qApp->applicationDirPath() + "/../src/app.qrc");
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "Unable to open resource file" << f.fileName() << f.errorString();
            return;
        }
        bool prefixFound = false;
        QXmlStreamReader xml(&f);
        while (!xml.atEnd()) {
            if (xml.readNext() == QXmlStreamReader::StartElement)
            {
                if (prefixFound and xml.name() == QLatin1String("file") and xml.attributes().value("alias") == alias)
                {
                    QFileInfo fi(qApp->applicationDirPath() + "/../src/" + xml.readElementText());
                    styleFile = fi.absoluteFilePath();
                    break;
                }
                else if (xml.name() == QLatin1String("qresource") and xml.attributes().value("prefix") == prefix)
                    prefixFound = true;
            }
            if (xml.hasError()) {
                qWarning() << "Failed to read resource file" << f.fileName() << xml.errorString();
                return;
            }
        }
        if (styleFile.isEmpty())
        {
            qWarning() << "Source file not found for resource" << cssResourcePath;
            return;
        }

        auto editor = new QPlainTextEdit;
        editor->setFont(CodeEditorFont().get());
        editor->setPlainText(browser->document()->defaultStyleSheet());
        Ori::Highlighter::setHighlighter(editor, ":/syntax/css");

        auto applyButton = new QPushButton("Apply");
        applyButton->connect(applyButton, &QPushButton::clicked, editor, [browser, editor]{
            browser->document()->setDefaultStyleSheet(editor->toPlainText());
        });

        auto saveButton = new QPushButton("Save");
        saveButton->connect(saveButton, &QPushButton::clicked, editor, [styleFile, editor]{
            QFile f(styleFile);
            if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qWarning() << "Unable to open file for writing" << styleFile << f.errorString();
                return;
            }
            f.write(editor->toPlainText().toUtf8());
            qDebug() << "Saved" << styleFile;
        });

        auto wnd = Ori::Layouts::LayoutV({
            new QLabel(styleFile),
            editor,
            Ori::Layouts::LayoutH({
                Ori::Layouts::Stretch(),
                applyButton,
                saveButton,
            }).setMargin(6)
        }).setMargin(3).setSpacing(6).makeWidget();
        wnd->setAttribute(Qt::WA_DeleteOnClose);
        wnd->setWindowTitle("Stylesheet Editor");
        wnd->setWindowIcon(QIcon(":/toolbar/protocol"));
        wnd->resize(300, 600);
        wnd->show();
    });
}

void editAppStyleSheet()
{
    auto editor = new QPlainTextEdit;
    editor->setFont(Z::Gui::CodeEditorFont().get());
    editor->setPlainText(Ori::Theme::loadRawStyleSheet());
    Ori::Highlighter::setHighlighter(editor, ":/syntax/qss");

    auto applyButton = new QPushButton("Apply");
    applyButton->connect(applyButton, &QPushButton::clicked, editor, [editor]{
        qApp->setStyleSheet(Ori::Theme::makeStyleSheet(editor->toPlainText()));
    });

    auto saveButton = new QPushButton("Save");
    saveButton->connect(saveButton, &QPushButton::clicked, editor, [editor]{
        auto res = Ori::Theme::saveRawStyleSheet(editor->toPlainText());
        if (!res.isEmpty()) Ori::Dlg::error(res);
    });

    auto wnd = Ori::Layouts::LayoutV({
        editor,
        Ori::Layouts::LayoutH({
            Ori::Layouts::Stretch(),
            applyButton,
            saveButton,
        }).setMargin(6).setSpacing(6)
    }).setMargin(3).setSpacing(0).makeWidget();
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowTitle("Style Sheet Editor");
    wnd->setWindowIcon(QIcon(":/toolbar/protocol"));
    wnd->resize(600, 600);
    wnd->show();
}

} // namespace Gui
} // namespace Z
