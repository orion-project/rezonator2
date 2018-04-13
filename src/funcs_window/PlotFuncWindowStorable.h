#ifndef PlotFuncWindowStorable_H
#define PlotFuncWindowStorable_H

#include "PlotFuncWindow.h"
#include "../io/ISchemaStorable.h"

class PlotFuncWindowStorable : public PlotFuncWindow, public ISchemaStorable
{
public:
    PlotFuncWindowStorable(PlotFunction* func);

    // implementation of  ISchemaStorable
    QString type() const override { return _function->alias(); }
    bool read(QJsonObject& root) override;
    void write(QJsonObject& root) override;

protected:
//    virtual bool readFunction(Z::IO::XML::Reader*, QDomElement&) { return true; }
//    virtual void writeFunction(Z::IO::XML::Writer*, QDomElement&) {}
//    virtual bool readWindowSpecific(Z::IO::XML::Reader*, QDomElement&) { return true; }
//    virtual void writeWindowSpecific(Z::IO::XML::Writer*, QDomElement&) {}

private:
//    bool readWindowGeneral(Z::IO::XML::Reader*, QDomElement&);
//    void writeWindowGeneral(Z::IO::XML::Writer*, QDomElement&) const;

//    void readTSMode(Z::IO::XML::Reader* reader, QDomElement& root);
//    void readCursor(Z::IO::XML::Reader* reader, QDomElement& root);
//    void writeTSMode(Z::IO::XML::Writer* writer, QDomElement& root) const;
//    void writeCursor(Z::IO::XML::Writer* writer, QDomElement& root) const;
};

#endif // PlotFuncWindowStorable_H
