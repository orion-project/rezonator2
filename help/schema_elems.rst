.. _schema_elems:

Elements List
=============

All elements containing in the schema are presented in the elements list. The elements list consist of several columns. Every element occupies its own row in the list, and each column contains following information about an element.

Columns
-------

Typ
~~~

The column shows a small image (icon) indicating an element type — distance, mirror, etc. With help of the :doc:`catalog`, you can clarify which image is corresponds to the specific element type.

Label
~~~~~

A short name or an :ref:`element's identifier <elem_props_label>`. That is how elements are commonly labeled on optical drawings.

Parameters
~~~~~~~~~~

A string describing magnitudes of all element's parameters. If some parameters are linked to :ref:`global parameters <params_window>`, then the global parameter label is also displayed in the row, e.g., `R = L1 = 100 mm` - this means that the parameter :param:`R` of the element is linked to the global parameter :param:`L1` and that gives the value `100mm`.

Title
~~~~~

A name or a :ref:`short description <elem_props_title>` of an element

Marker
~~~~~~

The column shows an icon indicating some additional information about the element.

The *exclamation icon* shows that some parameter of the element is in inconsistent state, probably because the global parameter it is :ref:`linked to <elem_props_link>` has an invalid formula. Hover the mouse over the icon to see a tooltip with the error message.

  .. image:: img/elems_table_invalid.png

The *stop icon* shows that the element is :ref:`blocked <elem_props_lock>`. If an element is blocked then it is not taken into account in round-trip calculation and it can not be chosen as function parameter.

  .. image:: img/elems_table_locked.png


Context Menu
------------

The context menu of the elements list includes :ref:`some commands <elem_opers>` for elements.

.. seeAlso::

  - :doc:`layout`
