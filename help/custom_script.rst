Custom Script
=============

:menuSelection:`Functions --> New Custom Script Window`

The Custom Script function allows you to perform arbitrary user calculations using the provided :doc:`py_api`. This powerful feature enables you to analyze your optical schema, perform custom computations, and process results according to your specific needs.

Code Module
-----------

A custom script is a user-defined Python module. The first line of the module docstring will be used as a human-readable function name and displayed in the function window headers. This allows you to give your script a meaningful title.

  .. note::
    Unlike :doc:`table <custom_table>` or :doc:`plot <custom_plot>` custom functions, where the script code is stored behind the respective view window, for custom script functions the script editor window is the only storage of your code. So if you close the editor window, then the function code *will be lost*.

``calculate()``
~~~~~~~~~~~~~~~

The script must contain a ``calculate`` function, which is automatically called without arguments when you press the :guilabel:`Run` button on the toolbar or use the :menuSelection:`Script --> Run` menu command. This method serves as the entry point for your custom calculations and will be executed each time you run the script.

.. code-block:: python

  def calculate():
    # Your calculation code here
    pass

Output
~~~~~~

All calculations in |rezonator| are performed in SI units (meters, radians, etc.). All parameter values provided to scripts are in SI units as well. For custom scripts that output to the log panel, you should manually format values to more convenient units for better readability. For example, convert meters to millimeters or nanometers when displaying wavelengths or beam sizes.

Custom code outputs results in text form to the log panel located in the bottom part of the script window. This is useful for displaying calculation results, debugging information, or status messages. To see output in the log panel, use the :ref:`rezonator.print() <py_module_z_print>` helper function rather than Python's built-in ``print()`` function. To clear the log panel, use :menuSelection:`Script --> Clear Log`.

For different kinds of output, consider using :doc:`custom_table` for tabular data presentation or :doc:`custom_plot` for graphical visualization of results.

Custom Library
--------------

You can save your code for later usage into a :ref:`custom code library <custom_func_lib>` using the :menuSelection:`Script --> Save to Custom Library` menu command. This allows you to build a collection of reusable calculation scripts for common tasks.

Custom Parameters
-----------------

:ref:`Custom parameters <custom_params>` are arbitrary parameters that you can add to any element in addition to its predefined parameters. They are especially useful for custom scripts because they allow you to calculate or track properties that are not covered by the element's default parameters.

You can add custom parameters to elements through the :doc:`elem_props` dialog and then access and manipulate them in your custom scripts using the :ref:`Element.param()<py_method_elem_param>` method. This provides flexibility to extend elements with project-specific data or intermediate calculation results.

.. seeAlso::

  - :doc:`py_api`
  - :doc:`custom_funcs`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :ref:`Example - Basic Ray Tracing<py_example_basic_ray_tracing>`
