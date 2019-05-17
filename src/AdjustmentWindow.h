#ifndef ADJUSTMENT_WINDOW_H
#define ADJUSTMENT_WINDOW_H

#include <QWidget>

#include "SchemaWindows.h"

class Schema;

class AdjustmentWindow : public QWidget, public SchemaToolWindow
{
    Q_OBJECT

public:
    static void open(Schema* schema, QWidget* parent = nullptr);
    ~AdjustmentWindow() override;

private:
    explicit AdjustmentWindow(Schema* schema, QWidget *parent = nullptr);
};

#endif // ADJUSTMENT_WINDOW_H
