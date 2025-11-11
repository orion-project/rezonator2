.. _elem_props_params:

Element Properties: Parameters
==============================

:menuSelection:`Element --> Properties --> Parameters`

Most of the elements have a list of parameters. Each parameter allows setting some physical characteristics of the element — the length, refraction index, and so on. This list is different for each element type. The description of the parameter is displayed in the bottom part of the list when a parameter is highlighted. The value of the parameter is given by a number and unit of measurement.

  .. image:: img/elem_props_params.png

Parameter expression
~~~~~~~~~~~~~~~~~~~~

Parameter value can also be given via simple expression. The expression can contain only numbers and basic mathematical functions (see :doc:`lua_primer`). When an expression is given, the actual parameter value is displayed in the tooltip when you hover the mouse over the input box. If the expression is invalid and can't be computed, the input box is displayed in red.


.. _elem_props_link:

Parameter links
~~~~~~~~~~~~~~~

The expression can not refer to other parameters. If you need to, then use the :menuSelection:`Link` button at the left of the input box. This opens a dialog for choosing a :ref:`global parameter <params_window>`. A link is created from the global parameter to the element's parameter. When the global parameter changes, its value is automatically assigned to the element's parameter, and all calculations get recomputed.

When parameter is linked to the global one, it can not be edit directly, it's value is displayed in the italic font. To remove the link, click the :menuSelection:`Link` button and select `(none)` in the dialog.

If the linked global parameter becomes an invalid, e.g., because its :ref:`formula <params_window_formula>` is incorrect, the input box gets marked red. A tooltip over the input box shows the global parameter error message. Elements having invalid parameters are marked with the exclamation icon in the elements table. A tooltip over the exclamation icon shows the global parameter error message.

  .. image:: img/elems_table_invalid.png

.. note::
  In this case it is up to the user's responsibility if a global parameter is used in the wrong context. For example, if you link the element's parameter :param:`L` (which is linear) to a global parameter having the angular dimension, the :param:`L` will probably get an unexpected value. When dimensions mismatch, the parameter value is assigned via SI conversion. For example, providing that the :param:`L` is displayed in cm, then linking it to an angular global parameter will get transformations like `10deg --> 0.1745rad --> 0.1745m --> 17.45cm`.

.. seeAlso::

    - :doc:`elem_props`
    - :ref:`Element Properties: Options <elem_props_options>`
    - :ref:`Element Properties: Outline <elem_props_outline>`
