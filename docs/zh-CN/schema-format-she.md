# reZonator-1 光学系统文件格式说明

光学系统（schema）文件的扩展名为 `*.she`。它们是 INI 格式的普通 ASCII 文本文件。

整体结构如下：

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

本文档仅描述 reZonator-2 加载光学系统所需的那部分节（section）和键（key）。

## 节 [PREFERENCES]

### Version
光学系统格式版本。支持 1、1.1 和 1.2 版本。

### Title
光学系统的标题，显示在窗口标题栏中。

### HasTitle
定义 `Title` 键是否有效。可以是 0 或 1。

### Notes
光学系统的文本备注，注释。

### SchemaKind
往返程（round trip）类型。

* 0 是 SW
* 1 是 RR
* 2 是 SP

### Wavelen
工作波长。对于 `Version=1`，它始终以纳米为单位，否则其单位由 `Units\Lambda` 键定义。

### LinearUnits
仅用于 `Version=1`。线性值的计量单位——元件的长度、焦距范围等。

* 0 是毫米
* 1 是微米

### AngleUnits
仅用于 `Version=1`。角度值的计量单位。

* 0 是度
* 1 是毫弧度
* 2 是角分
* 4 是弧度

**注意：** 此单位仅定义值在程序中如何显示。角度值在光学系统文件中始终**以弧度**存储。

### Units\Linear
用于 `Version` > 1。线性值的计量单位——元件长度、焦距范围等。支持的值有 `A`、`nm`、`mkm`、`mm`、`cm`、`m`。

### Units\Beamsize
用于 `Version` > 1。光束半径的计量单位。支持的值有 `A`、`nm`、`mkm`、`mm`、`cm`、`m`。

### Units\Lambda
用于 `Version` > 1。波长的计量单位。支持的值有 `A`、`nm`、`mkm`、`mm`、`cm`、`m`。

### Units\Angle
用于 `Version` > 1。角度值的计量单位。支持的值有 `rad`、`deg`、`mrad`、`min`。

**注意：** 此单位仅定义值在程序中如何显示。角度值在光学系统文件中始终**以弧度**存储。

### PumpType
仅用于 `Version=1`。定义 SP 光学系统的输入光束类型。

* 0 - 输入光束是高斯光束
* 1 - 输入光束是超高斯光束

### PumpSize
仅用于 `Version=1`。光束半径，以微米为单位。

### PumpParam1
仅用于 `Version=1`。当 `PumpType=1` 时的光束质量参数 MI。

### Pump\...
用于 `Version` > 1。SP 光学系统的输入光束参数。光束参数通过以下键分别为 T 平面和 S 平面定义

* `Pump\Param1T`, `Pump\Param1S`
* `Pump\Param2T`, `Pump\Param2S`
* `Pump\QualT`, `Pump\QualS`

这些键的含义取决于 `Pump\ParamsKind` 键的值。

#### Pump\ParamsKind=0
输入光束由其束腰半径定义。

* `Pump\Param1T`, `Pump\Param1S` - 束腰半径。
* `Pump\Param2T`, `Pump\Param2S` - 泵浦源到第一个光学系统元件的距离。
* `Pump\QualT`, `Pump\QualS` - 光束质量参数 MI。

#### Pump\ParamsKind=1
输入光束由其在第一个光学系统元件处的半径定义。

* `Pump\Param1T`, `Pump\Param1S` - 在第一个光学系统元件处的光束半径。
* `Pump\Param2T`, `Pump\Param2S` - 泵浦源到第一个光学系统元件的距离。
* `Pump\QualT`, `Pump\QualS` - 光束质量参数 MI。

#### Pump\ParamsKind=2 
输入光束由其复光束参数定义。

* `Pump\Param1T`, `Pump\Param1S` - 实部。
* `Pump\Param2T`, `Pump\Param2S` - 虚部。
* `Pump\QualT`, `Pump\QualS` - 光束质量参数 MI。

#### Pump\ParamsKind=3
输入光束由其倒数复光束参数定义。

* `Pump\Param1T`, `Pump\Param1S` - 实部。
* `Pump\Param2T`, `Pump\Param2S` - 虚部。
* `Pump\QualT`, `Pump\QualS` - 光束质量参数 MI。

#### Pump\ParamsKind=4
输入光束被定义为光线矢量，由其半径和半发散角给出。

* `Pump\Param1T`, `Pump\Param1S` - 在第一个光学系统元件处的光束半径。
* `Pump\Param2T`, `Pump\Param2S` - 发散角的一半。
* `Pump\QualT`, `Pump\QualS` - 不使用。

#### Pump\ParamsKind=5
输入光束被定义为光线矢量，由其两个半径给出。

* `Pump\Param1T`, `Pump\Param1S` - 在泵浦源处的光束半径。
* `Pump\Param2T`, `Pump\Param2S` - 在第一个光学系统元件处的光束半径。
* `Pump\QualT`, `Pump\QualS` - 泵浦源与第一个光学系统元件之间的距离。

## 节 [Element...]
这些节存储光学系统的元件。每个节名是 `Element` + 元件索引（从 1 开始）。

### Type
元件类型：

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
* TElemPoint
* TElemThinLensCylT (only for `Version=1`, --> TElemThinCylinderLensT)
* TElemThinLensCylS (only for `Version=1`, --> TElemThinCylinderLensS)
* TElemRange (only for `Version=1`, --> TElemPlate)
* TElemCustom (for `Version` < 1.2, --> TElemMatrix)
* TElemMatrix (for `Version` >= 1.2)

### Alias
元件标签。

### Title
元件标题。

### Disabled
可以是 0 或 1。如果 `Disabled=1`，该元件在计算中将被忽略。

### Param_...
元件参数的值。每个键名是 `Param_` + 参数名，例如 `Param_L`、`Param_n`。每个元件的参数名可以从元件目录（Element Catalog）中获取。

- 线性参数的值根据 `Version` 以 `LinearUnits` 或 `Units\Linear` 存储。

- 角度参数的值始终**以弧度**存储。

### Misalign\...
用于 `Version` > 1。元件失调。*reZonator-2 尚不支持。*


## 节 [WINDOWS]
函数窗口列表，格式如下：

```ini
Window...=<window type>
``` 

其中 `Window...` 定义要从中加载的节（见下文），`<window type>` 可以是以下之一：

* TwndGraph (only for `Version=1`)
* TwndGraphMultiCaustic (For `Version` > 1)
* ... **TODO** 待续 ...

*reZonator-2 尚不支持加载已存储的函数窗口。*

## 节 [Window...]
*reZonator-2 尚不支持加载已存储的函数窗口。*
