#ifndef COMMONDATA_H
#define COMMONDATA_H

#include <QObject>

namespace Ori {
    class Styler;
    class Translator;
    class MruFileList;
}

class CommonData : public QObject
{
    Q_OBJECT

public:
    explicit CommonData(QObject *parent = nullptr);
    ~CommonData();

    static CommonData* instance() { return _instance; }

    Ori::Styler* styler() { return _styler; }
    Ori::Translator* translator() { return _translator; }
    Ori::MruFileList* mruList() { return _mruList; }

    void addFileToMruList(const QString& fileName);

private:
    static CommonData* _instance;

    Ori::Styler* _styler;
    Ori::Translator* _translator;
    Ori::MruFileList* _mruList;
};

#endif // COMMONDATA_H
