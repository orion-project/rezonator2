# 光学系统文件格式

这里是关于不同光学系统（schema）格式的一些想法。

## INI

旧的基于 INI 的光学系统格式紧凑且易于阅读，但它缺乏嵌套对象结构等特性。

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

因此选择了新的基于 XML 的格式。它因为重复的标签名而不那么紧凑，并且需要额外的依赖 QtXml，但它本来就是框架的一部分。

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

它既可以很容易地被 Qt 类读取，也可以被其他框架读取（如有需要）——Python、JavaScript。与 XML 不同，它没有重复的标签，并且支持嵌套对象和数组。

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

[Google Protobuf](https://github.com/google/protobuf) 可能是最强大、最具描述性的格式，同时又很紧凑（尤其是在二进制格式下）。与 JSON 不同，它避免了到处都是大量的双引号和逗号。但作为明显的缺点，我们需要一个外部依赖来构建应用。不过它可以静态链接，因此运行时并不需要它。此外，为了充分发挥自动序列化/反序列化的能力，可能需要对应用的对象类做一些改动。

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

## 总结
目前，XML 已经足够好了，但 JSON 似乎可以是光学系统存储的最优格式。实际上优势并不算太多，因此应该再做一些实验，以弄清用这种格式是否能简化光学系统的加载/保存代码。

## 总结 +1
新的光学系统格式已改为 JSON。
