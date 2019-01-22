# Schema file format

There are some thoughts about different schema formats.

## INI

Old INI-based schema format is compact and easy to read, but it lacks features like nested objects structure.

```
[PREFERENCES]
Version=1.2
Title=Single lens
HasTitle=1
Notes=
Units\Linear=cm
Units\Beamsize=mm
Units\Lambda=mkm
Units\Angle=deg
SchemaKind=2
Wavelen=0.64

[Element0]
Type=TElemEmptyRange
Alias=L1
Title=Distance from light source to lens
Disabled=0
Param_L=5

[Element1]
Type=TElemThinLens
Alias=F1
Title=
Disabled=0
Param_F=5
Param_Alpha=0.261799387799149
Param_D=10

[Element2]
Type=TElemEmptyRange
Alias=L2
Title=
Disabled=0
Param_L=7
```

## XML

So new XML-based format was chosen. It's less compact because of repeated tag names and it requires additional dep QtXml, but it is a part of the framework anyway.

```
<rezonatorSchema version="2">
    <trip>SP</trip>
    <title>Single lens</title>
    <comment></comment>
    <lambda value="0.64" unit="mkm"/>
    <elements>
        <element type="ElemEmptyRange">
            <label>L1</label>
            <title>Distance from light source to lens</title>
            <options disabled="0"/>
            <params>
                <L value="5" unit="cm"/>
            </params>
        </element>
        <element type="ElemThinLens">
            <label>F1</label>
            <title></title>
            <options disabled="0"/>
            <params>
                <F value="5" unit="cm"/>
                <Alpha value="0.261799387799149" unit="deg"/>
            </params>
        </element>
        <element type="ElemEmptyRange">
            <label>L2</label>
            <title></title>
            <options disabled="0"/>
            <params>
                <L value="7" unit="cm"/>
            </params>
        </element>
    </elements>
</rezonatorSchema>  
```

## JSON

Can also be easily read as by Qt classes as from other frameworks if needed - Python, JavaScript. It has not repeated tags unlike XML and supports nested objects and arrays.

```
{
  "schema_version": "2",
  "trip_type": "SP",
  "title": "Single lens",
  "comment": "",
  "lambda": {
    "value": 0.64,
    "unit": "mkm"
  },
  "elements": [
    {
      "type": "ElemEmptyRange",
      "label": "L1",
      "title": "Distance from light source to lens",
      "is_disabled": 0,
      "params": [
        {
          "name": "L",
          "value": 5,
          "unit": "cm"
        }
      ]
    },
    {
      "type": "ElemThinLens",
      "label": "L1",
      "title": "",
      "params": [
        {
          "name": "F",
          "value": 5,
          "unit": "cm"
        },
        {
          "name": "Alpha",
          "value": 0.261799387799149,
          "unit": "deg"
        }
      ]
    },
    {
      "type": "ElemEmptyRange",
      "label": "L2",
      "title": "",
      "is_disabled": 0,
      "params": [
        {
          "name": "L",
          "value": 7,
          "unit": "cm"
        }
      ]
    }
  ]
}
```
## Protopbuf

[Google Protobuf](https://github.com/google/protobuf) may be the most powerful and descriptive format alongside with its compactness (especially in binary format). Unlike JSON it avoids a lot of double quotes and commas everywhere. But as the obvious drawback, we need an external dependency to build the app. But it can be linked statically, so it is not required at runtime. Also, some changes in app object classes can be required to involve a full power of automatical serialization/deserialization.

```
schema_version: "2"
trip_type: "SP"
title: "Single lens"
comment: ""
lambda {
  value: 0.64
  unit: "mkm"
}
element {
  type: "ElemEmptyRange"
  label: "L1"
  title: "Distance from light source to lens"
  is_disabled: 0
  param {
    name: "L"
    value: 5,
    unit: "cm"
  }
}
element {
  type: "ElemThinLens"
  label: "L1"
  title: ""
  param {
    name: "F"
    value: 5
    unit: "cm"
  },
  param {
    name: "Alpha"
    value: 0.261799387799149
    unit: "deg"
  }
}
element {
  type: "ElemEmptyRange"
  label: "L2"
  title: ""
  is_disabled: 0
  param {
    name: "L"
    value: 7
    unit: "cm"
  }
}
```

## Summary
Currently, XML is fine enough but seems that JSON can be optimal format for schema storage. There are not too many pros actually so additional experiment should be carried out to clarify if schema loading/saving code could be simplified with this format or not.

## Summary +1
New schema format was changed to JSON.
 