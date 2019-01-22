# reZonator-1 schema file format description

Schema files have `*.she` extension. They are general ASCII text files in INI format.

Overall structure is:

```ini
[section1]
key1=value1
key2=value2
...

[section2]
key1=value1
key2=value2
...
```

The document describes only part of sections and keys that are needed to load schemas by reZonator-2.

## Section [PREFERENCES]

### Version
Schema format version. Versions 1, 1.1 and 1.2 are supported.

### Title
Displayed title of the schema. *Not supported in reZonator-2*.

### HasTitle
Defines if `Title` key is vaid. Can be 0 or 1. *Not supported in reZonator-2*.

### Notes
Text notes for the schema, comment. *Not supported in reZonator-2*.

### SchemaKind
Round-trip type.

* 0 is SW
* 1 is RR
* 2 is SP

### Wavelen
Working wavelength. For `Version=1` it is always in nanometers, otherwise its unit is defined by `Units\Lambda` key.

### LinearUnits
Only for `Version=1`. Unit of measurement of linear values - elements' lengths, focus ranges, etc.

* 0 is millimeters
* 1 is micrometers

### AngleUnits
Only for `Version=1`. Unit of measurement of angular values.

* 0 is degrees
* 1 is milliradians
* 2 is angular minutes
* 4 is radians

**NB:** This unit only defines how values are displayed in the program. Angular values are always stored in schema files **in radians**.

### Units\Linear
For `Version` > 1. Unit of measurement of linear values - element length, focus range, etc. Supported values are `A`, `nm`, `mkm`, `mm`, `cm`, `m`.

### Units\Beamsize
For `Version` > 1. Unit of measurement of beams' radii. Supported values are `A`, `nm`, `mkm`, `mm`, `cm`, `m`.

### Units\Lambda
For `Version` > 1. Unit of measurement of wavelength. Supported values are `A`, `nm`, `mkm`, `mm`, `cm`, `m`.

### Units\Angle
For `Version` > 1. Unit of measurement of angular values. Supported values are `rad`, `deg`, `mrad`, `min`.

**NB:** This unit only defines how values are displayed in the program. Angular values are always stored in schema files **in radians**.

### PumpType
Only for `Version=1`. Defines input beam kind for SP schemas. 

* 0 - input beam is gaussisan beam
* 1 - input beam is hypergaussian beam

### PumpSize
Only for `Version=1`. Beam radius in micrometers.

### PumpParam1
Only for `Version=1`. Beam quality parameter MI when `PumpType=1`.

### Pump\\...
For `Version` > 1. Input beam parameters for SP schemas. Beam parameters are defined separately for T ans S planes via keys 

* `Pump\Param1T`, `Pump\Param1S`
* `Pump\Param2T`, `Pump\Param2S`
* `Pump\QualT`, `Pump\QualS`

Meaning of these keys depends on value of `Pump\ParamsKind` key.

#### Pump\ParamsKind=0
Input beam is defined by its waist radius.

* `Pump\Param1T`, `Pump\Param1S` - beam waist radius.
* `Pump\Param2T`, `Pump\Param2S` - distance of the pump source from the first schema element.
* `Pump\QualT`, `Pump\QualS` - beam quality parameter MI.

#### Pump\ParamsKind=1
Input beam is defined by its radius at the first schema element.

* `Pump\Param1T`, `Pump\Param1S` - beam radius at the first schema element.
* `Pump\Param2T`, `Pump\Param2S` - distance of the pump source from the first schema element.
* `Pump\QualT`, `Pump\QualS` - beam quality parameter MI.

#### Pump\ParamsKind=2 
Input beam is defined by its complex beam parameter.

* `Pump\Param1T`, `Pump\Param1S` - real part.
* `Pump\Param2T`, `Pump\Param2S` - imaginary part.
* `Pump\QualT`, `Pump\QualS` - beam quality parameter MI.

#### Pump\ParamsKind=3
Input beam is defined by its inverted complex beam parameter.

* `Pump\Param1T`, `Pump\Param1S` - real part.
* `Pump\Param2T`, `Pump\Param2S` - imaginary part.
* `Pump\QualT`, `Pump\QualS` - beam quality parameter MI.

#### Pump\ParamsKind=4
Input beam is defined as a ray vector by its radius and half-divergence ange.

* `Pump\Param1T`, `Pump\Param1S` - beam radius at the first schema element.
* `Pump\Param2T`, `Pump\Param2S` - half of divergence angle.
* `Pump\QualT`, `Pump\QualS` - not used.

#### Pump\ParamsKind=5
Input beam is defined as a ray vector by two of its radii.

* `Pump\Param1T`, `Pump\Param1S` - beam radius at the pump source.
* `Pump\Param2T`, `Pump\Param2S` - beam radius at the first schema element.
* `Pump\QualT`, `Pump\QualS` - distance between the pump source and the first schema element.

## Sections [Element...]
These sections store elements of the schema. Each section name is `Element` + element index (starting from 1).

### Type
Element type:

* TElemFlatMirror
* TElemCurveMirror
* TElemEmptyRange
* TElemMediaRange
* TElemPlate (for `Version` > 1)
* TElemBrewsterCrystal
* TElemBrewsterPlate
* TElemTiltedCrystal
* TElemTiltedPlate
* TElemThinLens
* TElemThinCylinderLensT (for `Version` > 1)
* TElemThinCylinderLensS (for `Version` > 1)
* TElemMatrix
* TElemPoint
* TElemThinLensCylT (only for `Version=1`, --> TElemThinCylinderLensT)
* TElemThinLensCylS (only for `Version=1`, --> TElemThinCylinderLensS)
* TElemRange (only for `Version=1`, --> TElemPlate)
* TElemCustom (for `Version` < 1.2, --> TElemMatrix)
* TElemMatrix (for `Version` >= 1)

### Alias
Element label.

### Title
Element title.

### Disabled
Can be 0 or 1. If `Disabled=1`, element will be ignored in calculation.

### Param_...
Values of element parameters. Each key name is `Param_` + parameter name, e.g. `Param_L`, `Param_n`. Names of parameters of each element can be picked up from Element Catalog.

- Values of linear parameters are stored in `LinearUnits` or `Units\Linear` depending on `Version`.

- Values of angular parameters are always stored **in radians**. 

### Misalign\\...
For `Version` > 1. Element misalignments. *Yet not supported in reZonator-2*.


## Section [WINDOWS]
List of function windows in format:

```ini
Window...=<window type>
``` 

Where `Window...` defines a section to load from (see below), and `<window type>` can be one of the follows:

* TwndGraph (only for `Version=1`)
* TwndGraphMultiCaustic (For `Version` > 1)
* ... **TODO** to be continued ...

*Loading of stored function windows is not supported in reZonator-2 yet.*

## Section [Window...]
*Loading of stored function windows is not supported in reZonator-2 yet.*
