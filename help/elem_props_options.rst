.. _elem_props_options:

Element Properties: Options
===========================

:menuSelection:`Element --> Properties --> Options`

  .. image:: img/elem_props_options.png

.. _elem_props_lock:

Ignore in calculation (disable element)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a flag is set, the element is ignored during round-trip calculation, and it is absent in the function argument pick list. It looks like the element is temporarily deleted from the schema.

  .. image:: img/elems_table_locked.png

TODO: lock-unlock command in the element list

Show element label on layout
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The option defines whether to show the element label on the :doc:`layout`. This is a cosmetic option allowing you to make a better view for layouts containing some type of element like :ref:`interfaces <matrix/ElemNormalInterface>` or :ref:`points <matrix/ElemPoint>`.

For example, here is a layout of the “singlet_thick_intf” example with hidden labels for :doc:`matrix/ElemSphericalInterface` elements representing lens surfaces:

  .. image:: img/layout_hide_labels.png

Draw narrow version of element
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some element types, like :doc:`matrix/ElemEmptyRange` or :doc:`matrix/ElemTiltedInterface`, can have a narrower representation on the :doc:`layout`. This is a cosmetic option allowing you to make a better view for layouts containing some type of elements.

For example, here is a layout of the “z_crystal” example, where the :param:`L3` is displayed in the narrow mode, which reflects the fact that it's significantly shorter than the :param:`L2` range:

  .. image:: img/layout_narrow_elem.png

.. seeAlso::

    :doc:`elem_props`,
    :ref:`Element Properties: Parameters <elem_props_params>`,
    :ref:`Element Properties: Outline <elem_props_outline>`
