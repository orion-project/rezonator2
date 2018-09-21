#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

class ElementRangeView : public ElementView
{
public:
    ElementRangeView() { HW = 50; HH = 5; }
    virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
    qreal length() const { return 2*HW; }
    void setLength(const qreal &value) { HW = value / 2.0; }
};


#endif // LAYOUTVIEW_H
