#ifndef PLOT_FUNC_WINDOW_V2_H
#define PLOT_FUNC_WINDOW_V2_H

#include "../io/ISchemaWindowStorable.h"
#include "../math/PlotFunctionV2.h"
#include "../windows/PlotBaseWindow.h"

class FrozenStateButton;
class PlotFunctionV2;
class UnitsMenu;

/**
    Simplified variant of @sa PlotFuncWindow storing all graph lines in one plain list.

    @sa PlotFuncWindow and @sa PlotFunction should be considered obsolete
    and all new plot functions should be implemented via @sa PlotFunctionV2 and PlotFuncWindowV2
*/
class PlotFuncWindowV2 : public PlotBaseWindow, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    explicit PlotFuncWindowV2(PlotFunctionV2*);
    ~PlotFuncWindowV2() override;

    PlotFunctionV2* function() const { return _function; }

    /// Do autolimits after next update.
    void requestAutolimits() { _autolimitsRequest = true; }

    /// Cursor should be ceneterd after next update.
    void requestCenterCursor() { _centerCursorRequested = true; }

    /// Edits function parameters through dialog.
    bool configure();

    // Inherited from BasicMdiChild
    QString helpTopic() const override { return _function->helpTopic(); }

    // Inherited from IAppSettingsListener
    void optionChanged(AppSettingsOption option) override;

    // Implementation of ISchemaWindowStorable
    QString storableType() const override { return _function->alias(); }
    bool storableRead(const QJsonObject& root, Z::Report* report) override;
    bool storableWrite(QJsonObject& root, Z::Report* report) override;

    // Implementation of SchemaListener
    void recalcRequired(Schema*) override { update(); }
    void elementDeleting(Schema*, Element*) override;
    void globalParamDeleting(Schema*, Z::Parameter*) override;
    void customParamDeleting(Z::Parameter*) override;

    virtual bool shouldCloseIfDelete(const Elements&);

public slots:
    void update();

protected:
    PlotFunctionV2* _function;
    FrozenStateButton* _buttonFrozenInfo;
    Z::Unit _unitX = Z::Units::none();
    Z::Unit _unitY = Z::Units::none();
    UnitsMenu *_unitsMenuX, *_unitsMenuY;
    bool _autolimitsRequest = false; ///< If autolimits requested after next update.
    bool _centerCursorRequested = false; ///< If cursor should be centered after next update.
    bool _needRecalc = false;
    bool _frozen = false;
    QAction *actnUpdate, *actnFreeze, *actnFrozenInfo;
    QHash<QString, QPen> _graphPens;
    QList<QCPGraph*> _graphs;
    
    // Inherited from PlotBaseWindow
    void graphFormatted(QCPGraph*) override;

    virtual bool configureInternal() { return true; }
    virtual void beforeUpdate() {}
    virtual void afterUpdate() {}
    virtual Z::Unit getDefaultUnitX() const { return Z::Units::none(); }
    virtual Z::Unit getDefaultUnitY() const { return Z::Units::none(); }

    virtual QString readFunction(const QJsonObject&) { return QString(); }
    virtual QString writeFunction(QJsonObject&) { return QString(); }
    virtual QString readWindowSpecific(const QJsonObject&) { return QString(); }
    virtual QString writeWindowSpecific(QJsonObject&) { return QString(); }

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void updateGraphs();
    void updateStatusUnits();
    void showStatusError(const QString &message);
    void clearStatusInfo();

    void disableAndClose();

    Z::Unit getUnitX() const;
    Z::Unit getUnitY() const;
    void setUnitX(Z::Unit unit);
    void setUnitY(Z::Unit unit);

    QPen graphPen(const QString &name) const;
    
    void clearGraphs();
    
private slots:
    void freeze(bool);
    
private:
    QString readWindowGeneral(const QJsonObject& root, Z::Report *report);
    QString writeWindowGeneral(QJsonObject& root) const;
};

#endif // PLOT_FUNC_WINDOW_V2_H
