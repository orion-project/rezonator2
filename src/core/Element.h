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
        QString type() const override { return QStringLiteral(# class_name); }\
        static QString _type_() { return QStringLiteral(# class_name); }


#define DECLARE_ELEMENT_END };

#define DEFAULT_LABEL(label)\
    QString labelPrefix() const override { return QStringLiteral(label); }

#define TYPE_NAME(name)\
    QString typeName() const override { static QString _name_ = name; return _name_; }\
    static QString _typeName_() { static QString _name_ = name; return _name_; }

#define CALC_MATRIX\
    void calcMatrixInternal() override;

#define SUB_RANGE\
    void calcSubmatrices() override;

#define CHECK_PARAM\
    const char* checkParameter(Z::Parameter *param, double newValue) const;

#define AXIS_LEN\
    double axisLengthSI() const override;

// IOR is internal parameter by default,
// and should be explicitly revealed by derived elements
#define ELEM_PROLOG_RANGE\
    _kind = ElementKind::Range; \
    _matrs[MatrixKind::T1] = Z::Matrix(); \
    _matrs[MatrixKind::S1] = Z::Matrix(); \
    _matrs[MatrixKind::T2] = Z::Matrix(); \
    _matrs[MatrixKind::S2] = Z::Matrix(); \
    _length =  new Z::Parameter(Z::Dims::linear(), \
                                QStringLiteral("L"), QStringLiteral("L"), \
                                qApp->translate("Param", "Length")); \
    _ior = new Z::Parameter(Z::Dims::none(), \
                            QStringLiteral("n"), QStringLiteral("n"), \
                            qApp->translate("Param", "Index of refraction")); \
    _length->setValue(100_mm); \
    _ior->setVisible(false); \
    _ior->setValue(1); \
    addParam(_length); \
    addParam(_ior);

#define ELEM_PROLOG_DYNAMIC\
    _kind = ElementKind::Dynamic; \
    _matrs[MatrixKind::DynT] = Z::Matrix(); \
    _matrs[MatrixKind::DynS] = Z::Matrix();
    
// IOR parameters can't be directly assigned,
// their values are taked from neighboub range elements
// so they are invisible for parameter editors
#define ELEM_PROLOG_INTERFACE\
    _kind = ElementKind::Interface; \
    _ior1 = new Z::Parameter(Z::Dims::none(), \
                             QStringLiteral("n1"), QStringLiteral("n1"), \
                             qApp->translate("Param", "Index of refraction (left medium)")); \
    _ior2 = new Z::Parameter(Z::Dims::none(), \
                             QStringLiteral("n2"), QStringLiteral("n2"), \
                             qApp->translate("Param", "Index of refraction (right medium)")); \
    _ior1->setVisible(false); \
    _ior2->setVisible(false); \
    _ior1->setValue(1); \
    _ior2->setValue(1); \
    addParam(_ior1); \
    addParam(_ior2); \
    setOption(Element_Asymmetrical); \
    setOption(Element_ChangesWavefront); \
    layoutOptions.showLabel = false; \
    

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
    
    /// Element has complex matrices.
    /// I'm not sure if complex matrixes are valid for the ABCD-method.
    /// There is a modified ABCDGH-method for all-complex elements,
    /// but it requires a lot of work and a bit more literature to adopt it.
    Element_Complex = 0x20,
};

struct ElementLayoutOptions
{
    bool showLabel = true;

    /// Draw an alternative version of the element.
    /// For some elements the alternative version is a narrower one
    /// which can be useful when schema contains many elements.
    /// In general, how the option is processed depends on particular element type.
    /// @sa ElementLayoutOptionsView, @sa ElementLayoutFactory::getOptions()
    bool drawAlt = false;
};

enum class MatrixKind {
    T,
    S,
    InvT,
    InvS,
    DynT,
    DynS,
    T1,
    S1,
    T2,
    S2,
};

enum class ElementKind {
    Simple, ///< Generic element without particular treatment
    Range, ///< Elements having length and optional IOR
    Dynamic, ///< Elements whose matrix depends on beam parameters
    Interface, ///< Elements whose matrix depend on IOR of neighbour elements
};

struct DynamicElemCalcParams
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

struct ElemAsRangeImpl;
using ElemAsRange = std::optional<ElemAsRangeImpl>;
struct ElemAsDynamicImpl;
using ElemAsDynamic = std::optional<ElemAsDynamicImpl>;
struct ElemAsInterfaceImpl;
using ElemAsInterface = std::optional<ElemAsInterfaceImpl>;

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
    
    ElementKind kind() const { return _kind; }

    /// Function returns type of element, e.g. "ElemFlatMirror".
    /// Type is used for internal identification of element class like true class name.
    virtual QString type() const = 0;

    /// Function returns "human-friendly" name of element type, e.g. "Flat mirror".
    virtual QString typeName() const { return type(); }

    /// Default prefix for generating of automatical labels for elements of this type.
    virtual QString labelPrefix() const { return QString(); }

    const Z::Parameters& params() const { return _params; }
    bool hasParams() const { return !_params.isEmpty(); }
    bool hasParam(Z::Parameter* param) const { return _params.contains(param); }
    Z::Parameter* param(const QString &alias) { return _params.byAlias(alias); }
    void addParam(Z::Parameter* param, int index = -1);
    void removeParam(Z::Parameter* param, bool free);
    void moveParamUp(Z::Parameter* param);
    void moveParamDown(Z::Parameter* param);
    /// Put the same parameters in different order.
    /// It's supposed that the given params array contains the same parameters
    /// as the element already has - their pointers taken from params editor.
    void reorderParams(const Z::Parameters& params);

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
    
    const Z::Matrix& M(MatrixKind kind) const { return _matrs.at(kind); }
    const Z::Matrix* pM(MatrixKind kind) const { return &_matrs.at(kind); }

    const Z::Matrix& Mt() const { return M(MatrixKind::T); }
    const Z::Matrix& Ms() const { return M(MatrixKind::S); }
    const Z::Matrix* pMt() const { return pM(MatrixKind::T); }
    const Z::Matrix* pMs() const { return pM(MatrixKind::S); }
    const Z::Matrix& Mt_inv() const { return M(MatrixKind::InvT); }
    const Z::Matrix& Ms_inv() const { return M(MatrixKind::InvS); }
    const Z::Matrix* pMt_inv() const { return pM(MatrixKind::InvT); }
    const Z::Matrix* pMs_inv() const { return pM(MatrixKind::InvS); }
    
    bool hasMatrix(MatrixKind kind) const { return _matrs.contains(kind); }

    bool disabled() const { return _disabled; }
    void setDisabled(bool value);

    void setOption(ElementOption option) { _options |= option; }
    bool hasOption(ElementOption option) const { return _options & option; }

    bool failed() const;
    QString failReason() const;
    
    virtual QList<QPair<Z::Parameter*, Z::Parameter*>> flip() { return {}; }

    ElementLayoutOptions layoutOptions;
    
    bool isRange() const { return _kind == ElementKind::Range; }
    bool isDynamic() const { return _kind == ElementKind::Dynamic; }
    bool isInterface() const { return _kind == ElementKind::Interface; }
    ElemAsRange asRange();
    ElemAsDynamic asDynamic();
    ElemAsInterface asInterface();

protected:
    Element();

    ElementOwner* _owner = nullptr; ///< Pointer to an object who owns this element.
    ElementKind _kind = ElementKind::Simple;
    QString _label, _title;
    std::map<MatrixKind, Z::Matrix> _matrs;
    int _id;
    bool _disabled = false;
    Z::Parameters _params;
    int _options = 0;
    QString _error;

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
    
    // Support for ElementKind::Range functionality
    Z::Parameter *_length = nullptr;
    Z::Parameter *_ior = nullptr;
    double _subRangeSI;
    double lengthSI() const { return _length->value().toSi(); }
    double ior() const { return _ior->value().value(); }
    /// Unlike crystals or rods (e.g. ElemBrewsterCrystal, ElemTiltedCrystal),
    /// in tilted plates (e.g. ElemBrewsterPlate, ElemTiltedPlate),
    /// a value of the Length parameter is different
    /// from the beam's path it travels inside the plate.
    /// The function should account the plate's angle
    /// and return a geometrical distance between the beam's
    /// input and output points at the plate's edges.
    virtual double axisLengthSI() const { return lengthSI(); }
    virtual void calcSubmatrices() {}
    double opticalPathSI() const { return axisLengthSI()* ior(); }
    friend class ElemAsRangeImpl;

    // Support for ElementKind::Dynamic functionality
    virtual void calcDynamicMatrix(const DynamicElemCalcParams& p) { Q_UNUSED(p) }
    friend class ElemAsDynamicImpl;
    
    // Support for ElementKind::Interface functionality
    // Elements representing an interface between two media.
    // An interface element is characterized by two IORs - `ior1` and `ior2`.
    // Where `ior1` is IOR of a medium at 'the left' of the interface (medium 1),
    // and `ior2` is IOR of a medium at 'the right' of the interface (medium 2).
    Z::Parameter *_ior1 = nullptr;
    Z::Parameter *_ior2 = nullptr;
    double ior1() const { return _ior1->value().value(); }
    double ior2() const { return _ior2->value().value(); }
    friend class ElemAsInterfaceImpl;
};

typedef QList<Element*> Elements;

// To be able to use in `QVariant::value<Element*>(data)`
Q_DECLARE_METATYPE(Element*);

//------------------------------------------------------------------------------
/**
    Accessor for elements having length and optional IOR.
*/
struct ElemAsRangeImpl
{
    Element *elem;
    
    void setSubRangeSI(double value) { elem->_subRangeSI = value; elem->calcSubmatrices(); }
    void setSubRange(const Z::Value& value);
    double subRangeSI() const { return elem->_subRangeSI; }
    Z::Value subRangeLf() const;
    Z::Value subRangeRt() const;

    const Z::Matrix& Mt1() const { return elem->M(MatrixKind::T1); }
    const Z::Matrix& Ms1() const { return elem->M(MatrixKind::S1); }
    const Z::Matrix& Mt2() const { return elem->M(MatrixKind::T2); }
    const Z::Matrix& Ms2() const { return elem->M(MatrixKind::S2); }
    const Z::Matrix* pMt1() const { return elem->pM(MatrixKind::T1); }
    const Z::Matrix* pMs1() const { return elem->pM(MatrixKind::S1); }
    const Z::Matrix* pMt2() const { return elem->pM(MatrixKind::T2); }
    const Z::Matrix* pMs2() const { return elem->pM(MatrixKind::S2); }
    
    double axisLengthSI() const { return elem->axisLengthSI(); }
    Z::Parameter* paramLength() const { return elem->_length; }
    Z::Parameter* paramIor() const { return elem->_ior; }
    double lengthSI() const { return elem->lengthSI(); }
    double ior() const { return elem->ior(); }

    /// Returns element's axial length (@a Element::axisLengthSI())
    /// as a Z::Value with unit that is used for the Length parameter.
    Z::Value axisLen() const;

    double opticalPathSI() const { return elem->opticalPathSI(); }
};

//------------------------------------------------------------------------------
/**
    Accessor to an element representing an interface between two media.
    An interface element is characterized by two IORs - `ior1` and `ior2`.
    Where `ior1` is IOR of a medium at 'the left' of the interface (medium 1),
    and `ior2` is IOR of a medium at 'the right' of the interface (medium 2).
*/
struct ElemAsInterfaceImpl
{
    Element *elem;

    Z::Parameter* paramIor1() const { return elem->_ior1; }
    Z::Parameter* paramIor2() const { return elem->_ior2; }
};

//------------------------------------------------------------------------------

/**
    Accessor to an element for interpreting it as a Dynamic element.
    Dynamic element is an elements whose matrix depends on beam parameters.
*/
struct ElemAsDynamicImpl
{
    Element *elem;
  
    void calcDynamicMatrix(const DynamicElemCalcParams& p) { elem->calcDynamicMatrix(p); }

    const Z::Matrix& Mt_dyn() const { return elem->M(MatrixKind::DynT); }
    const Z::Matrix& Ms_dyn() const { return elem->M(MatrixKind::DynS); }
    const Z::Matrix* pMt_dyn() const { return elem->pM(MatrixKind::DynT); }
    const Z::Matrix* pMs_dyn() const { return elem->pM(MatrixKind::DynS); }
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

class ElementParamsBackup
{
public:
    ElementParamsBackup(Element *elem, const char *reason);
    ~ElementParamsBackup();

private:
    Element *_elem;
    QHash<Z::Parameter*, std::shared_ptr<Z::ParamValueBackup>> _backup;
    const char *_reason;
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
