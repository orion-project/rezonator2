.. _params_window:

Params Window
=============

.. index:: single: global parameter
.. index:: single: schema parameter

:menuSelection:`Window --> Parameters (F11)`

Schema project can store a number of parameters which are independent from element so they called "global parameters". Their values can be assigned directly or can depend on other parameters and be expressed via formula (see below).

Global parameters can be used for providing values for element parameters and this gives an ability to make interconnected and driven elements in the schema - when you change a value of some global parameter and this instantly affects several elements at once.

  .. image:: img/params_window.png


.. _params_window_create:

Create New Parameter
--------------------

:menuSelection:`Parameter --> Create... (Ctrl+Ins)`

When creating a new parameter, one should specify its Name and Dimension, these two are mandatory and can not be changed later. While Unit only takes a default value, actual unit of measurement can be changed later when specifying parameter magnitude.

  .. image:: img/params_window_create.png


.. _params_window_annotate:

Annotate Parameter
------------------

:menuSelection:`Parameter --> Annotate... (Ctrl+Return)`

Annotation is just a short human readable description of parameter.

  .. image:: img/params_window_annotate.png


.. _params_window_value:

Set Parameter Value
-------------------

:menuSelection:`Parameter --> Set... (Return)`

Parameter value can be assigned directly via dialog. You can override unit of measurement specified during the parameter construction.

  .. image:: img/params_window_set.png


.. _params_window_formula:

Set Parameter Formula
---------------------

:menuSelection:`Parameter --> Set... (Return) --> Add formula`

Parameter value can be derived from values of other parameters. You can provide a formula for value calculation. In the "Set value" dialog click the right bars menu and select the :menuSelection:`Add formula` item. Later, use the same menu and the command :menuSelection:`Remove formula` if you don't need formula any longer and want to provide parameter value directly.

  .. image:: img/params_window_formula_1.png

A dialog appears where you can type an expression that will be calculated to obtain the parameter value. Formulas are written in Lua syntax (see :doc:`lua_primer`). While you type, the formula is instantly checked for correctness and a red marker shows if something is wrong with it.

  .. image:: img/params_window_formula_2.png

The formula can depend on values of other parameters, it will be automatically recomputed and a new parameter value obtained when one of dependencies changes. Dependencies must be stated explicitly in the "Params" page of the dialog. Click the Plus button and select a parameter to dependent on. Several dependencies can be added one by one.

  .. image:: img/params_window_formula_3.png

When dependencies are registered, you can use them by name in the formula code to derive the parameter value from. Here in the picture :param:`L0` and :param:`F0` are names of other parameters added as dependencies:

  .. image:: img/params_window_formula_4.png

When parameter is driven by formula, it is marked by the "gear" icon in the table and the list of dependencies is shown in the "Name" column. E.g. :param:`L1 = f(L0, F0)` means that a value for parameter :param:`L1` is somehow calculated from values of parameters :param:`L0` and :param:`F0` which are also here in the table.

  .. image:: img/params_window_formula_5.png

.. _params_window_adjust:

Adjust Parameter
----------------

:menuSelection:`Parameter --> Adjust`

Global parameter can be used in the :doc:`Adjustment tool<adjust>` like any other element parameter. Just select the :menuSelection:`Adjust` command from the parameters table context menu or use the :menuSelection:`Adjust` command in the window menu. But if the parameter is driven by formula it can not be really adjusted because you can't provide a value to the driven parameter directly (see that value of :param:`L1` is marked in italic font in the picture below). Instead you have to adjust its dependencies and the Adjuster tool will reflect how the driven value is changed. 

  .. image:: img/params_window_adjust.png


Delete  Parameter
-----------------

:menuSelection:`Parameter --> Delete (Ctrl+Del)`

The system checks if a parameter under deletion is a dependency for another global parameter or if there is an element parameter linked to it and prevents deletion of such parameter. All dependencies and links should be refused manually.
