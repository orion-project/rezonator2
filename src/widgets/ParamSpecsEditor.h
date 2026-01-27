#ifndef PARAM_SPECS_EDITOR_H
#define PARAM_SPECS_EDITOR_H

#include <QObject>

#include "../core/Parameters.h"

class Element;
class ParamsEditor;
class Schema;

class ParamSpecsEditor : public QObject
{
    Q_OBJECT
    
public:
    explicit ParamSpecsEditor(Element *element, ParamsEditor *editorParams, bool customParams, QObject *parent = nullptr);
    ~ParamSpecsEditor();

    void collect();
    void reject();

    void createParamDlg();
    void editParamDlg();
    void removeParamDlg();
    void moveParamUp();
    void moveParamDown();
    void addParam(Z::Parameter *param);
    
signals:
    void onSavePreset(Z::Parameter *param);

private:
    Element *_element;
    ParamsEditor *_editorParams;
    Z::Parameters _newParams, _removedParams, _editedParams, _redimedParams;
    QMap<Z::Parameter*, Z::Parameter> _backupParams;
    bool _customParams;
    bool _reordered = false;
    
    Z::Parameters existedParams() const;
};

#endif // PARAM_SPECS_EDITOR_H
