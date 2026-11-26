# 基本代码风格规则

* 类型名应使用驼峰命名法（camel case），例如 `CalcManager`、`TripTypeInfo`

* 源代码文件名应使用驼峰命名法，例如 `SchemaClient.h`、`ElementsCatalog.cpp`

* 目录名应使用小写字母加下划线，例如 `src/func_windows`。

* 源代码文件应以其包含的主要类型命名，例如 `Schema.h` 和 `Schema.cpp` 文件包含 `Schema` 类的实现。这些文件也包含许多其他类型，但主要的类型是 `Schema`。

* 实现某种实体（无论是 UI 元素还是核心类型）的类型，应以后缀标明该实体的类型，例如 `ParamsListWidget`、`FrozenStateButton`、`MultiCausticFunction`
