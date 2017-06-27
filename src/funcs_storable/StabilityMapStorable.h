#ifndef STORABLE_STABILITY_MAP_H
#define STORABLE_STABILITY_MAP_H

#include "../funcs/FunctionStorable.h"

class StabilityMapWindow;

class StabilityMapStorable : public FunctionStorable
{
public:
    static SchemaStorable* constructor(Schema*);
    StabilityMapStorable(StabilityMapWindow* wnd);

    StabilityMapWindow* window() const { return _window; }

protected:
    void writeFunction(Z::IO::XML::Writer*, QDomElement&) override;
    bool readFunction(Z::IO::XML::Reader*, QDomElement&) override;

    void writeWindow(Z::IO::XML::Writer*, QDomElement&) override {}
    bool readWindow(Z::IO::XML::Reader*, QDomElement&) override { return true; }

private:
    StabilityMapWindow* _window;
};

#endif // STORABLE_STABILITY_MAP_H
