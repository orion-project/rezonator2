Table Window
============

Results of calculations of certain functions are represented as a data table in a special table window. E.g. :doc:`func_beamdata`.

  .. image:: img/func_beamdata.png

Table Operations
----------------

:menuSelection:`Table --> Update (F5)`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The command calculates the function again. The command is inaccessible if the current function is :doc:`frozen<func_freeze>`.


:menuSelection:`Table --> Freeze (Ctrl+F)`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The command freezes the current table window. See :doc:`func_freeze` for details.


:menuSelection:`Table --> Show T-plane`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The command toggles visibility of results for the tangential (T) plane. 


:menuSelection:`Table --> Show S-plane`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The command toggles visibility of results for the sagittal (S) plane. 

.. note::
  One of the result sets (for T or S plane) always stays visible. It means if one of the sets is hidden and you try to hide the other one, then the first becomes visible. 


:menuSelection:`Table --> Options --> Calculate at medium ends`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate function in :doc:`matrix/ElemMediaRange` elements. This is a debug option. In general, it's important to know beam parameters at interfaces surrounding the medium.

  .. image:: img/wnd_table_calc_medium_ends.png


:menuSelection:`Table --> Options --> Calculate in empty spaces`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate function values in :doc:`matrix/ElemEmptyRange` elements. This is a debug option. In a common case, an empty space separates some ‘rigid’ elements: lenses, crystals, etc. The function value on those elements will be the same as the values on the adjoined ends of the neighboring empty space.

  .. image:: img/wnd_table_calc_empty_spaces.png


:menuSelection:`Table --> Options --> Calculate in the middle of ranges`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate function in the middle of :doc:`matrix/ElemEmptyRange` and :doc:`matrix/ElemMediaRange` elements. This is a debug option.

.. seeAlso::

    :doc:`table_symbols`, :doc:`plot_window`, :doc:`info_window`
