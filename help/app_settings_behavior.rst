Application Settings: Behavior
==============================

:menuSelection:`Edit --> Settings --> Behavior`

.. --------------------------------------------------------------------------

.. _app_settings_edit_just _created:

Edit just created element
~~~~~~~~~~~~~~~~~~~~~~~~~

Automatically open the :doc:`elem_props` dialog after a new element has been added to the schema from the :doc:`catalog`.

.. --------------------------------------------------------------------------

.. _app_settings_gen_labels_created:

Generate labels for created elements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the option is set, then a :ref:`label <elem_props_label>` will be automatically assigned to each :ref:`newly created <elem_opers_append>` element. Each :ref:`element type <elem_matrs>` has its own label prefix. The label consists of this prefix followed by an incrementing index. For example, labels have a format `L1`, `L2`... for ranges and `M1`, `M2`... for mirrors.

.. --------------------------------------------------------------------------

.. _app_settings_gen_labels_pasted:

Generate labels for pasted elements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the option is set, then new :ref:`labels <elem_props_label>` will be automatically assigned to elements that are :ref:`pasted <elem_opers_paste>` from the clipboard. This can be useful because elements copied from another schema can have labels that conflict with the labels of the existing elements.

.. --------------------------------------------------------------------------

Generate labels for new pumps
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the option is set, then a label will be automatically assigned to each newly created :ref:`pump <pumps_window>`. The label consists of the prefix `P` followed by an incrementing index, e.g., `P1`, `P2`.

.. --------------------------------------------------------------------------

Show protocol window after application started
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is a debug option that allows the log window to be automatically shown after the application starts. This window can also be displayed manually via the command :menuSelection:`Window --> Protocol`.

.. --------------------------------------------------------------------------

.. _app_settings_show_py_code:

Show Python code for matrices in info windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The option allows for displaying element matrices as NumPy variables in the :ref:`Element Matrices <elem_show_matrix>` window. This is used for testing and debugging.

.. --------------------------------------------------------------------------

Don't load function windows when opening schema
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

With this option you can disable loading of all function windows saved in the schema file. Use this option to reduce the time of schema loading, but in this case you have to recalculate all required functions again. The options also can be useful if loading of saved function windows is interrupted with an error, for example, if they had been saved in a different version of the program.

.. --------------------------------------------------------------------------

Open documentation online instead of from local file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It's been `reported <https://github.com/orion-project/rezonator2/issues/9>`_ that the local documentation sometimes fails to show. If this happens, the `online manual <http://rezonator.orion-project.org/help>`_ can be used instead.

.. --------------------------------------------------------------------------
