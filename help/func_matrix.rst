.. _func_matrix:

Element Matrices
================

:menuSelection:`Element --> Show Matrix`

:menuSelection:`Element --> Show All Matrices`

  .. image:: img/func_matrix.png

The window shows the ray matrix of one or several elements selected on the :doc:`schema_elems`.

If an element has different matrices for forward and back :ref:`propagation <propagation_dir>`, like the :doc:`matrix/ElemThickLens` has, for example, they both are displayed.

In addition to the user-friendly table view, the window can show matrices as Python NumPy variables. It is not used by default and should be enabled in the :ref:`application settings <app_settings_show_py_code>`. This is a debug feature that can be used for testing scripts. It works in combination with the :doc:`func_rt` functions, which also can show Python code for round-trip multiplication using the same variable names.

.. seeAlso::

    :doc:`info_window`