.. _elem_props:

Element Properties
==================

:menuSelection:`Element --> Properties`

Common properties
-----------------

  .. image:: img/elem_props_common.png

.. _elem_props_label:

Label
~~~~~

It is an identifier or label of an element. Labels generally consist of several letters, for example, `M1` or `L_foc`. That is how elements are commonly labeled on optical drawings.

Label can be generated automatically at element creation if it's allowed by the corresponding :ref:`application option <app_settings_gen_labels_created>`.  Each :ref:`element type <elem_matrs>` has its own label prefix. The label consists of this prefix followed by an incrementing index. For example, labels have a format `L1`, `L2`... for ranges and `M1`, `M2`... for mirrors.


.. _elem_props_title:

Title
~~~~~

It is a name of the element or its short description. For example, "Output mirror". This field has totally informational meaning.


Property tabs
-------------

There is an additional set of properties that vary depending on the element type.

.. tocTree::
  :maxDepth: 1

  elem_props_params
  elem_props_options
  elem_props_outline
