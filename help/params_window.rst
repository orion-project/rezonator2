.. _params_window:

Params Window
=============

.. index:: single: global parameter
.. index:: single: schema parameter

:menuSelection:`Window --> Parameters (F11)`

A schema project can store a number of parameters independent from elements, so they called "global parameters." Their values can be assigned directly or can depend on other parameters and be expressed via formula (see below).

Global parameters can be used for providing values for :ref:`element parameters <elem_props_link>`, and this gives an ability to make interconnected and driven elements in the schema. Then you can change a value of some global parameter, and this instantly affects several elements at once.

  .. image:: img/params_window.png

.. --------------------------------------------------------------------------

.. _params_window_create:

Create New Parameter
--------------------

:menuSelection:`Parameter --> Create... (Ctrl+Ins)`

When creating a new parameter, one should specify its Name, Dimension, and Description. Unit of measurement can be set later when specifying parameter magnitude.

  .. image:: img/params_window_create.png

.. --------------------------------------------------------------------------

.. _params_window_edit:

Edit Parameter
------------------

:menuSelection:`Parameter --> Edit... (Ctrl+Return)`

When editing a parameter, it’s possible to change its name. Then the name is automatically updated in elements and formulas that use this parameter.

It’s also possible to change the parameter dimension. In this case it is up to the user's responsibility if the parameter is used in the wrong context. For example, if you change the dimension of the F0 parameter to be “Angular,” then the lens elements having their focal ranges linked to the F0 probably will take incorrect values. When dimensions mismatch, the parameter value is assigned via SI conversion. For example, providing that the focal range linked to the F0 is displayed in cm, changing F0 from “Linear” to “Angular,” we will get transformations like `10deg --> 0.1745rad --> 0.1745m --> 17.45cm`. 

Description is just a short human-readable description of the parameter, and its changing only affects what’s displayed in the table.

  .. image:: img/params_window_edit.png

.. --------------------------------------------------------------------------

.. _params_window_value:

Set Parameter Value
-------------------

:menuSelection:`Parameter --> Set... (Return)`

Parameter value can be assigned directly via dialog.

  .. image:: img/params_window_set.png


Parameter value can also be given via simple expression. The expression can contain only numbers and basic mathematical functions (see :doc:`lua_primer`). It can not refer to other parameters. If you need to, then use formulas instead of expressions; see below. When an expression is given, the actual parameter value is displayed in the tooltip when you hover the mouse over the input box.

  .. image:: img/params_window_set_expr.png

.. --------------------------------------------------------------------------

.. _params_window_formula:

Set Parameter Formula
---------------------

:menuSelection:`Parameter --> Set... (Return) --> Add formula`

Parameter value can be derived from values of other parameters. You can provide a formula for value calculation. In the "Set value" dialog, click the right bars menu and select the :menuSelection:`Add formula` item. Later, use the same menu and the command :menuSelection:`Remove formula` if you don't need the formula any longer and want to provide the parameter value directly.

  .. image:: img/params_window_formula_1.png

A dialog appears where you can type an expression that will be calculated to obtain the parameter value. Formulas are written in Lua syntax (see :doc:`lua_primer`). While you type, the formula is instantly checked for correctness, and a red marker shows if something is wrong with it.

  .. image:: img/params_window_formula_2.png

Formula can depend on values of other parameters. Just type the name of another parameter, and the dependency will be automatically created. Here in the picture, :param:`L0` and :param:`F0` are names of other parameters added as dependencies. When one of the dependencies changes, the dependent parameter gets automatically recomputed respecting the new value.

When a parameter is driven by a formula, it is marked by the “gear” icon in the table, and the list of dependencies is shown in the “Name” column. E.g. :param:`L1 = f(L0, F0)` means that a value for parameter :param:`L1` is somehow calculated from values of parameters :param:`L0` and :param:`F0`, which are also here in the table.

.. --------------------------------------------------------------------------

.. _params_window_adjust:

Adjust Parameter
----------------

:menuSelection:`Parameter --> Adjust`

A global parameter can be used in the :doc:`Adjustment tool<adjust>` like any other element parameter. Just select the :menuSelection:`Adjust` command from the parameters table context menu or use the :menuSelection:`Adjust` command in the window menu. But if the parameter is driven by a formula it can not be really adjusted because you can’t provide a value to the driven parameter directly (see that the value of :param:`L1` is marked in italic font in the picture below). Instead, you have to adjust its dependencies, and the Adjuster tool will reflect how the driven value is changed.

  .. image:: img/params_window_adjust.png

.. note::
  If an expression had been set for the parameter value, after using the adjuster it will be reset and replaced with the actual value. Because the adjuster can not know what part of the expression should be changed, so it changes the whole value.

Delete  Parameter
-----------------

:menuSelection:`Parameter --> Delete (Ctrl+Del)`

The system checks if a parameter under deletion is a dependency for another global parameter or if there is an element parameter linked to it and prevents deletion of such a parameter. All dependencies and links should be refused manually.
