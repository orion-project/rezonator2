# Basic code style rules

* Type names should be in camel case, e.g. `CalcManager`, `TripTypeInfo`

* Cource code file names should be in camel case, e.g. `SchemaClient.h`, `ElementsCatalog.cpp`

* Directory names should be in lower case with underscores, e.g. `src/func_windows`.

* Source code files shoule be named after the main type they contain, e.g. `Schema.h` and `Schema.cpp` files contain implementation of `Schema` class. These files also contain a number of additional types, but the main is `Schema`.

* Types implementing some entity (wether UI element or core type) should be suffixed with type of this entity, e.g. `ParamsListWidget`, `FrozenStateButton`, `MultiCausticFunction`
