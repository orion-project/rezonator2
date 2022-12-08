.. index:: single: freezing

Function Freezing
=================

:menuselection:`Plot --> Freeze`

:menuselection:`Table --> Freeze`

By default, the content of most function windows (function results) is automatically refreshed when you change parameters of schema or some elements. The freezing disables this automatic recalculation for a function. 

**Example of usage:** consider you have a :doc:`contour stability map<func_stabmap_2d>` and you've been choosing operational conditions for resonator by investigating this map. You iteratively change parameter of an element (by directly editing :doc:`element parameters<elem_props>` or with help of :doc:`adjustment<adjust>`) which is one of variable parameters (arguments) of the stability map. And you observe, for instance, how :doc:`caustic<func_caustic>` inside some element is altered when you change a parameter's value. The stability map will also be recalculated each time but its appearance obviously stays the same since you change its variable parameter. Repeated computing of contour stability map can take a long while if it was plotted rather detailed. In this case you can freeze the stability map window to disable unnecessary recalculation of it.

Function freezing can be done with help of respective button on the function window toolbar or by means of the command "Freeze" in the function window menu. When a function is frozen, a special button appeared on the function window toolbar. It is called "Frozen info" and contains information about parameters' values of all elements at the moment of freezing, so called 'frozen information'. 

    .. image:: img/func_freeze.png

.. TODO: When schema is saved, the calculated data of frozen function widows and frozen information are saved into schema file. When such file is loaded into resonator, frozen functions are not calculated but its frozen data are just displayed. 

.. TODO: Note:  not all functions provide saving of frozen data in current version of program. Help topic for certain function informs if function does not provide this ability. In such case saved frozen function windows will be empty (containing no data) after schema file will be loaded. 

.. seealso::

    :doc:`functions`
