Table Operations
================

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Edit
---------

.. _table_opers_copy:

Copy
~~~~

The command copies the content of selected table cells into the clipboard. If there are position icons int the cells, they are copied in text representation. See :doc:`table_symbols`.

.. --------------------------------------------------------------------------

Select All
~~~~~~~~~~

Select all cells in the current table window.

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Table
----------

.. --------------------------------------------------------------------------

Update
~~~~~~

:menuSelection:`Table --> Update (F5)`

The command calculates the function again. The command is inaccessible if the current function is :doc:`frozen<func_freeze>`.

.. --------------------------------------------------------------------------

Freeze
~~~~~~

:menuSelection:`Table --> Freeze (Ctrl+F)`

The command freezes the current table window. See :doc:`func_freeze` for details.

.. --------------------------------------------------------------------------

Show T-plane
~~~~~~~~~~~~

:menuSelection:`Table --> Show T-plane`

The command toggles visibility of results for the tangential (T) plane. 

  .. note::
    One of the result sets (for T or S plane) always stays visible. It means if one of the sets is hidden and you try to hide the other one, then the first becomes visible. 

.. --------------------------------------------------------------------------

Show S-plane
~~~~~~~~~~~~

:menuSelection:`Table --> Show S-plane`

The command toggles visibility of results for the sagittal (S) plane. 

  .. note::
    One of the result sets (for T or S plane) always stays visible. It means if one of the sets is hidden and you try to hide the other one, then the first becomes visible. 

.. --------------------------------------------------------------------------

Calculate at medium ends
~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Table --> Options --> Calculate at medium ends`

Calculate function in :doc:`matrix/ElemMediaRange` elements. This is a debug option. In general, it's important to know beam parameters at interfaces surrounding the medium.

  .. image:: img/table_calc_medium_ends.png


.. --------------------------------------------------------------------------

Calculate in empty spaces
~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Table --> Options --> Calculate in empty spaces`

Calculate function values in :doc:`matrix/ElemEmptyRange` elements. This is a debug option. In a common case, an empty space separates some ‘rigid’ elements: lenses, crystals, etc. The function value on those elements will be the same as the values on the adjoined ends of the neighboring empty space.

  .. image:: img/table_calc_empty_spaces.png

.. --------------------------------------------------------------------------

Calculate in the middle of ranges
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Table --> Options --> Calculate in the middle of ranges`

Calculate function in the middle of :doc:`matrix/ElemEmptyRange` and :doc:`matrix/ElemMediaRange` elements. This is a debug option.

.. --------------------------------------------------------------------------

.. seeAlso::

  :doc:`table_window`, :doc:`table_symbols`
