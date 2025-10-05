#ifndef ELEMENT_H
#define ELEMENT_H

#include "Math.h"
#include "Parameters.h"

#include <optional>

#include <QSize>

#define DECLARE_ELEMENT(class_name, base_class)\
    class class_name : public base_class\
    {\
    protected:\
        Element* create() const override { return new class_name(); }\
    public:\
        const QString type() const override { return QStringLiteral(# class_name); }\
        static const QString _type_() { return QStringLiteral(# class_name); }


#define DECLARE_ELEMENT_END };

#define DEFAULT_LABEL(label)\
    const QString labelPrefix() const override { return QStringLiteral(label); }

#define TYPE_NAME(name)\
    const QString typeName() const override { static QString _name_ = name; return _name_; }\
    static const QString _typeName_() { static QString _name_ = name; return _name_; }

#define PARAMS_EDITOR(editor)\
    Z::ParamsEditorKind paramsEditorKind() const override { return Z::ParamsEditorKind::editor; }

#define CALC_MATRIX\
    void calcMatrixInternal() override;

#define SUB_RANGE\
    void calcSubmatrices() override;

#define CHECK_PARAM\
    const char* checkParameter(Z::Parameter *param, double newValue) const;

#define AXIS_LEN\
    double axisLengthSI() const override;

class Element;
class PumpCalculator;

//------------------------------------------------------------------------------
/**
    Base class for objects who wish to own optical elements.
*/
class ElementOwner
{
public:
    enum Position {PositionInvalid, PositionAtLeft, PositionInMidle, PositionAtRight};
public:
    virtual ~ElementOwner();
    virtual void elementChanged(Element*, Z::ParameterBase*, const QString &reason) { Q_UNUSED(reason) }
    virtual int indexOf(Element*) const { return -1; }
    virtual int count() const { return 0; }
    virtual Position position(Element*) const { return PositionInvalid; }
    friend class Element;
};

//------------------------------------------------------------------------------

enum ElementOption {
    /// The element can calculate two sets of matrices -
    /// one for the forward propagation and another for the back propagation.
    /// This options is used only for output and formatting
    /// in order not to show equal matrices twice.
    /// Each element must initialize back propagation matrices explicitly
    /// event when they are the same as forward propagation ones.
    Element_Asymmetrical = 0x01,

    /// The element can change wavefront, so functions calculating something
    /// at elements (e.g., Beam Parameters at Element) should calculate
    /// before and after such an element to provide full information.
    /// There is no reason to set this option for range-like elements
    /// or interface elements because they treated separately.
    Element_ChangesWavefront = 0x02,

    /// The element is a sample for creation of other elements.
    /// Such samples are stored in the Custom Elements Library and shown
    /// in the Elements Catalog on a separate page.
    Element_CustomSample = 0x04,

    /// Wavelength must be passed to the element to calculate matrices.
    /// Schema takes care of that, it listens for wavelength changes
    /// and passes new lambda value to all elements having this option set.
    /// The element must provide parameter "Lambda" for accepting wavelength.
    Element_RequiresWavelength = 0x08,
    
    /// This is a helper element with unity matrix (like Point)
    /// It does nothing besides of marking some position in the schema
    /// or splitting two ranges
    Element_Unity = 0x10,
};

struct ElementLayoutOptions {
    bool showLabel = true;

    /// Draw an alternative version of the element.
    /// For some elements the alternative version is a narrower one
    /// which can be useful when schema contains many elements.
    /// In general, how the option is processed depends on particular element type.
    /// @sa ElementLayoutOptionsView, @sa ElementLayoutFactory::getOptions()
    bool drawAlt = false;
};

/**
    Base class for all optical elements.

    Each element has two set of matrices - one for forward propagation
    and other for back propagtion (named `*-inv` matrices). Back propagation process
    only involved in SW schemas where beam travels each element (but the endings) twice:

    ```
           \\|       forward propagation             |//
       end \\| ====================================> |// end
    mirror \\|-----[//]------[\\]----()----[\\\]-----|// mirror
           \\| <==================================== |//
           \\|         back propagation              |//
    ```

    Most of the elements are symmetrical and inverted set of matrices are the same as the forward set.
    But there are several elements having these sets different (@see ThickLens, interface elements).
    They have option @a Element_Asymmetrical.
*/
class Element : public Z::ParameterListener
{
public:
    ~Element() override;

    ElementOwner* owner() const { return _owner; }
    void setOwner(ElementOwner *owner);

    int id() const { return _id; }

    /// Function returns type of element, e.g. "ElemFlatMirror".
    /// Type is used for internal identification of element class like true class name.
    virtual const QString type() const = 0;

    /// Function returns "human-friendly" name of element type, e.g. "Flat mirror".
    virtual const QString typeName() const { return type(); }

    /// Default prefix for generating of automatical labels for elements of this type.
    virtual const QString labelPrefix() const { return QString(); }

    const Z::Parameters& params() const { return _params; }
    bool hasParams() const { return !_params.isEmpty(); }
    bool hasParam(Z::Parameter* param) const { return _params.contains(param); }
    Z::Parameter* param(const QString &alias) { return _params.byAlias(alias); }
    void addParam(Z::Parameter* param, int index = -1);
    void removeParam(Z::Parameter* param);

    /// Label of element. Label is short indentificator
    /// for element or its name (like variable name). E.g.: "M1", "L_f", etc.
    const QString& label() const { return _label; }
    void setLabel(const QString& value);

    /// User title of element.
    /// E.g.: "Output coupler", "Folding mirror", etc.
    const QString& title() const { return _title; }
    void setTitle(const QString& value);

    /// Returns element label, or element index if the label is empty.
    QString displayLabel();

    /// Returns element title, or displayLabel() if the title is empty.
    QString displayTitle();

    /// Returns element title and label as "{elem_label} ({elem_title})".
    /// Or only one of thoses if the other is empty, or number and type
    /// of element if both label and title are empty.
    QString displayLabelTitle();

    void calcMatrix(const char* reason);

    const Z::Matrix& Mt() const { return _mt; }
    const Z::Matrix& Ms() const { return _ms; }
    const Z::Matrix* pMt() const { return &_mt; }
    const Z::Matrix* pMs() const { return &_ms; }
    const Z::Matrix& Mt_inv() const { return _mt_inv; }
    const Z::Matrix& Ms_inv() const { return _ms_inv; }
    const Z::Matrix* pMt_inv() const { return &_mt_inv; }
    const Z::Matrix* pMs_inv() const { return &_ms_inv; }

    /// Preferable parameter editor kind for this element.
    virtual Z::ParamsEditorKind paramsEditorKind() const { return Z::ParamsEditorKind::List; }

    bool disabled() const { return _disabled; }
    void setDisabled(bool value);

    void setOption(ElementOption option) { _options |= option; }
    bool hasOption(ElementOption option) const { return _options & option; }

    virtual std::optional<Z::Value> aperture() const { return {}; }

    bool failed() const;
    QString failReason() const;

    ElementLayoutOptions layoutOptions;

protected:
    Element();

    ElementOwner* _owner = nullptr; ///< Pointer to an object who owns this element.
    QString _label, _title;
    Z::Matrix _mt, _ms;
    Z::Matrix _mt_inv, _ms_inv;
    int _id;
    bool _disabled = false;
    Z::Parameters _params;
    int _options = 0;

    virtual void calcMatrixInternal();

    void parameterChanged(Z::ParameterBase*) override;
    void parameterFailed(Z::ParameterBase*) override;

    bool _calcMatrixLocked = false;
    bool _calcMatrixNeeded = false;
    friend class ElementMatrixLocker;

    int _eventsLocked = false;
    friend class ElementEventsLocker;

    /// Support for ElementsCatalog's functionality
    friend class ElementsCatalog;
    virtual Element* create() const = 0;
};

typedef QList<Element*> Elements;

// To be able to use in `QVariant::value<Element*>(data)`
Q_DECLARE_METATYPE(Element*);

//------------------------------------------------------------------------------
/**
    The base class for elements having length and optional IOR.
*/
class ElementRange : public Element
{
public:
    void setSubRangeSI(double value) { _subRangeSI = value; calcSubmatrices(); }
    void setSubRange(const Z::Value& value);
    double subRangeSI() const { return _subRangeSI; }
    Z::Value subRangeLf() const;
    Z::Value subRangeRt() const;

    const Z::Matrix& Mt1() const { return _mt1; }
    const Z::Matrix& Ms1() const { return _ms1; }
    const Z::Matrix& Mt2() const { return _mt2; }
    const Z::Matrix& Ms2() const { return _ms2; }
    const Z::Matrix* pMt1() const { return &_mt1; }
    const Z::Matrix* pMs1() const { return &_ms1; }
    const Z::Matrix* pMt2() const { return &_mt2; }
    const Z::Matrix* pMs2() const { return &_ms2; }

    Z::Parameter* paramLength() const { return _length; }
    Z::Parameter* paramIor() const { return _ior; }
    double lengthSI() const { return _length->value().toSi(); }
    double ior() const { return _ior->value().value(); }

    virtual double axisLengthSI() const { return lengthSI(); }
    virtual double opticalPathSI() const { return axisLengthSI()* ior(); }
    Z::Value axisLen() const;

protected:
    ElementRange();

    Z::Matrix _mt1, _mt2;
    Z::Matrix _ms1, _ms2;
    Z::Parameter *_length;
    Z::Parameter *_ior;
    double _subRangeSI;

    virtual void calcSubmatrices() {}
};

//------------------------------------------------------------------------------
/**
    The base class for elements representing an interface between two media.
    An interface element is characterized by two IORs - `ior1` and `ior2`.
    Where `ior1` is IOR of a medium at 'the left' of the interface (medium 1),
    and `ior2` is IOR of a medium at 'the right' of the interface (medium 2).
*/
class ElementInterface : public Element
{
public:
    Z::Parameter* paramIor1() const { return _ior1; }
    Z::Parameter* paramIor2() const { return _ior2; }
    double ior1() const { return _ior1->value().value(); }
    double ior2() const { return _ior2->value().value(); }

protected:
    ElementInterface();

    Z::Parameter *_ior1, *_ior2;
};

//------------------------------------------------------------------------------
/**
    The base class for elements whose matrix depends on beam parameters.
*/
class ElementDynamic : public Element
{
public:
    struct CalcParams
    {
        /// Matrices of part of the schema
        /// from the first element up to this element (not including).
        const Z::Matrix *Mt, *Ms;

        /// Propagating beam calculators incapsulate input beam parameters
        /// and can compute output beam parameters from ray matrix.
        PumpCalculator *pumpCalc;

        /// Schema wevelength in meters.
        double schemaWavelenSi;

        /// We don't care if this IOR differs from IOR of the current element (in the case it has IOR).
        /// In this case the beam transition between elements is invalid, but it is up to user.
        double prevElemIor;
    };

    virtual void calcDynamicMatrix(const CalcParams& p) { Q_UNUSED(p) }

    const Z::Matrix& Mt_dyn() const { return _mt_dyn; }
    const Z::Matrix& Ms_dyn() const { return _ms_dyn; }
    const Z::Matrix* pMt_dyn() const { return &_mt_dyn; }
    const Z::Matrix* pMs_dyn() const { return &_ms_dyn; }

protected:
    Z::Matrix _mt_dyn, _ms_dyn;

    void calcMatrixInternal() override;
};

//------------------------------------------------------------------------------
/**
    The class prevents element from generating the 'modified' event
    every time when a parameter value changes.
*/
class ElementEventsLocker
{
public:
    ElementEventsLocker(Element *elem, const char *reason);
    ElementEventsLocker(Z::Parameter *param, const char *reason);
    ~ElementEventsLocker();

private:
    Elements _elems;
    const char *_reason;

    void collectElems(Z::Parameter *param);
};

//------------------------------------------------------------------------------

class ElementMatrixLocker
{
public:
    ElementMatrixLocker(Element* elem, const char* reason): _elem(elem), _reason(reason)
    {
        _elem->_calcMatrixLocked = true;
    }

    ~ElementMatrixLocker()
    {
        _elem->_calcMatrixLocked = false;

        if (_elem->_calcMatrixNeeded)
        {
            _elem->_calcMatrixNeeded = false;
            _elem->calcMatrix(_reason);
        }
    }

private:
    Element *_elem;
    const char *_reason;
};

//------------------------------------------------------------------------------
//                                Z::Utils

namespace Z {
namespace Utils {

inline bool isRange(const Element *elem) { return dynamic_cast<const ElementRange*>(elem); }
inline ElementRange* asRange(Element *elem) { return dynamic_cast<ElementRange*>(elem); }
inline bool isInterface(Element *elem) { return dynamic_cast<ElementInterface*>(elem); }
inline ElementInterface* asInterface(Element *elem) { return dynamic_cast<ElementInterface*>(elem); }

void setElemWavelen(Element* elem, const Z::Value& lambda);

/// Gives a filter of parameters for regular users' usage.
/// These are parameters that can be edited in Element properties dialog,
/// or they can be selected as functions' arguments.
ParameterFilterPtr defaultParamFilter();

/// Copyes parameter values from source element to the target one.
/// Both elements should be of the same type or at least contain the same number of parameters.
void copyParamValues(const Element* source, Element* target, const char *reason);

/// Copyes parameter values from source element to the target one.
/// Elements can be of different types, parameters are matched by their name and dimention.
void copyParamValuesByName(const Element* source, Element* target, const char *reason);

inline QSize elemIconSize() { return QSize(24, 24); }
inline QString elemIconPath(const QString& elemType) { return ":/elem_icon/" % elemType; }
inline QString elemIconPath(Element* elem) { return elemIconPath(elem->type()); }
inline QString elemDrawingPath(const QString& elemType) { return ":/elem_drawing/" % elemType; }

QString displayStr(const Elements &elems);

} // namespace Utils
} // namespace Z

#endif // ELEMENT_H
