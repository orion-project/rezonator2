.. _func_freeze:
.. index:: single: freezing

Function Freezing
=================

:menuSelection:`Plot --> Freeze`

:menuSelection:`Table --> Freeze`

By default, the content of most function windows (function results) is automatically refreshed when you change parameters of schema or some elements. The freezing disables this automatic recalculation for a function. 

**Example of usage:** Consider you have a :doc:`2D stability map<func_stabmap_2d>` and you've been choosing operational conditions for the resonator by investigating this map. You iteratively change a parameter of an element (by directly editing :doc:`element parameters<elem_props>` or with the help of :doc:`adjustment<adjust>`) which is one of the variable parameters (arguments) of the stability map. And you observe, for instance, how :doc:`caustic<func_caustic>` inside some element is altered when you change a parameter's value. The stability map will also be recalculated each time, but its appearance obviously stays the same since you change its variable parameter. Repeated computing of a 2D stability map can take a long while if it was plotted rather detailed. In this case you can freeze the stability map window to disable unnecessary recalculation of it.

Function freezing can be done with the help of the respective button on the function window toolbar or by means of the command :menuSelection:`Freeze` in the function window menu. When a function is frozen, a special button appears on the function window toolbar. It is called "Frozen Info" and contains information about parameters' values of all elements at the moment of freezing, so-called 'frozen information'. 

    .. image:: img/func_freeze.png

.. TODO: When schema is saved, the calculated data of frozen function widows and frozen information are saved into schema file. When such file is loaded into resonator, frozen functions are not calculated but its frozen data are just displayed. 

.. TODO: Note:  not all functions provide saving of frozen data in current version of program. Help topic for certain function informs if function does not provide this ability. In such case saved frozen function windows will be empty (containing no data) after schema file will be loaded. 

    .. note::
        Currently, the frozen state is not saved into the schema project file. So when you reopen the projects, all frozen functions are recalculated and shown as usual.

.. seeAlso::

    :doc:`functions`
