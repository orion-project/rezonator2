.. index:: single: adjustment

Schema Adjustment
=================

:menuSelection:`Tool --> Adjustment`

:menuSelection:`Element context menu --> Add Adjuster --> [Parameter name]`

Adjustment tool allows for observation of influence of some parameter value to schema operation and for selection of its optimal value. The overall approach is you define one or several adjusters for different parameters, then you click the control buttons Plus/Minus/Multiply/Divide to slightly vary the adjusted parameter and you look at how the function plots and tables change. This should resemble as if you were slightly shifting different verniers and handles in your system and observing how this affects indications of various probes.

A window displays a list of user defined **adjusters**. Adjuster contains a reference to an element parameter or to a :ref:`global schema parameter <params_window>` (called "adjusted parameter", a parameter under adjustment) and :ref:`adjustment settings<adjust_settings>` defining how severe the adjusted parameter value changed by each adjusting iteration.

  .. image:: img/adjust.png

.. _adjust_add:

Create Adjuster
---------------

When you open the Adjustment window first time it will be empty, you have to click the :guiPart:`Add Adjuster` button to create first adjuster. A dialog appears where it's possible to choose an element and its parameter or a global schema parameter to make an adjuster for. 

Parameters whose values drawn in italic font are read-only because they are not directly editable and their values are calculated somehow (e.g. linked to a global schema parameter, or driven by formula). It's possible to add adjuster to such parameters too, it will not be editable but will reflect changes of underlying value. To really change the parameter you need to create adjuster for its dependencies instead, they are listed after parameter name. For example in the picture below parameter F1.F is shown as "F = F0 = 10cm" and this means it really takes value from the global parameter F0 so you have to adjust that instead of F1.F. Similarly, global parameter dL is shown as "dL = f(F0) = 5mm" and this means it depends on the the global parameter F0 too, so you again have to adjust that to get changes in dL.

  .. image:: img/adjust_add.png

A convenient way to add new adjuster is to use the element context menu, then it is possible to quickly select an element and parameter to adjust and the Adjustment window will be opened with an adjuster for this parameter automatically created. If there was already opened window then a new adjuster is added to that. Similar context menu is also available for :ref:`global schema parameters <params_window_adjust>`.

  .. image:: img/adjust_add_1.png

.. _adjust_settings:

Adjuster Settings
-----------------

Adjuster settings define how severe the adjusted parameter's value is changed in each adjusting iteration.

  .. image:: img/adjust_settings.png

**Increment**

It defines the count of units to increase or decrease adjusted parameter value when you click Plus or Minus buttons in the Adjustment window. This value is set in the same units of measurements as the adjusted parameter has. For example, when you select an adjuster for a parameter having current magnitude 100mm then the increment 1 means 1mm. While when you select another adjuster for an angular parameter then the increment 1 means 1deg or 1mrad depending on what unit was selected for the parameter.

**Multiplier**

It defines amount of times to increase or decrease adjusted parameter value when you click Multiply or Divide buttons in the Adjustment window. It is not required but makes sense to set the Multiplier to a value greater than 1. Otherwise when you click Multiply button the adjusted parameter value will actually decrease which is confusing.

**Set as default values**

If the flag is set then the provided settings will be stored permanently and used for all newly created adjusters. Not only in the current Adjustment window but in any future windows and schemas.

**Apply for all adjuster**

If the flag is set then settings will be applied to all adjusters in the window, not only to selected one.
