#ifndef EXAMPLES_DIALOG_H
#define EXAMPLES_DIALOG_H

#include <QSplitter>

class QLineEdit;
class QListWidget;
class QTextBrowser;

class ExamplesDialog : public QSplitter
{
    Q_OBJECT

public:
    ExamplesDialog();

    QString selectedFile() const;

    static QString exec();
    
signals:
    void accepted();
    
private:
    QLineEdit *_filterEdit;
    QListWidget *_fileList;
    QTextBrowser *_preview;
    QString _examplesDir;
    QHash<QString, QString> _descrs;
    QHash<QString, QString> _plainTextDescrs;
    
    void loadExamples();
    void showCurrentExample();
    void applyFilter();
    QJsonObject loadExampleFile(const QString& fileName);
    QString loadExampleDescr(const QString& fileName);
    void editExampleDescr();
};

#endif // EXAMPLES_DIALOG_H
