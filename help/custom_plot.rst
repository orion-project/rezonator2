Custom Plot Function
====================

:menuSelection:`Functions --> Create Custom Plot Function`

The Custom Plot Function allows you to perform arbitrary user calculations using the provided :doc:`py_api` and output the results as graphs in a :doc:`plot window <plot_window>` rather than in text form like the simpler :doc:`custom_script` function does.  This powerful feature enables you to analyze your optical schema, perform custom computations, and visualize results graphically, making it ideal for visualizing dependencies, trends, and relationships between optical parameters.

For custom functions, the plot window shows an additional command :menuSelection:`Plot --> Show Code` and the respective button with the Python icon on the toolbar. The command opens a code editor window to change the code of your script. This window can be safely closed because the code is actually stored in the plot window, not in the code window. However, if you close the plot window itself, then the function code *will be lost*.

Code Module
-----------

A custom function is a user-defined Python module. The first line of the module docstring will be used as a human-readable function name and displayed in the function window headers. This allows you to give your custom plot function a meaningful title. In the plot's title, this name will be substituted instead of the ``{func_name}`` text variable.

``figure()``
~~~~~~~~~~~~

The code module must contain a ``figure()`` function that configures the plot properties. This function is called before each recalculation, which allows you to dynamically change plot properties (such as axis titles based on selected elements) without reopening the window.

.. code-block:: python

  def figure():
    return {
      'x_dim': Z.DIM_LINEAR,
      'x_title': 'Position',
      'y_dim': Z.DIM_LINEAR,
      'y_title': 'Beam radius'
    }

The function should return a dictionary with the following keys:

- ``x_dim`` - Value dimension for the X-axis (:ref:`Z.DIM_LINEAR <py_const_dim_linear>`, :ref:`Z.DIM_ANGULAR <py_const_dim_angular>`, :ref:`Z.DIM_NONE <py_const_dim_none>`)
- ``x_title`` - Default title for the X-axis. It can include element labels or parameter descriptions and will be substituted instead of the ``{default_title}`` text variable.
- ``y_dim`` - Value dimension for the Y-axis
- ``y_title`` - Default title for the Y-axis

``calculate()``
~~~~~~~~~~~~~~~

The code must also contain a ``calculate()`` function that performs the actual calculations and generates the data to plot. This function is called without arguments when you press the :guilabel:`Run` button, or use the :menuSelection:`Script --> Run` menu command, or refresh the plot window via :menuSelection:`Plot --> Update`.

.. code-block:: python

  def calculate():
    # Perform calculations
    x_values = [...]
    y_values = [...]
    return [
      {
        'label': 'My graph',
        'x': x_values,
        'y': y_values
      }
    ]

The function should return a list of graph line data, where each line is represented as a dictionary with keys:

- ``label`` - A name displayed in the legend; also, it is a key for combining several lines into a single multi-segment graph
- ``x`` - List or array of X-coordinate values
- ``y`` - List or array of Y-coordinate values (must have the same length as ``x``)

Output
~~~~~~

All calculations in |rezonator| are performed in SI units (meters, radians, etc.). All parameter values provided to scripts are in SI units as well. For custom plot functions, you should return calculated X and Y values in SI units without any conversion. Because the plot window has built-in capability to choose and display units according to the axes' dimension types (``x_dim`` and ``y_dim`` properties defined in the ``figure()`` function). The user can select preferred units through the plot window interface.

The results from the ``calculate()`` function are automatically displayed as graphs in a :doc:`plot_window`. You can return multiple dictionaries to display several lines on the same plot. Dictionaries with the same ``label`` become parts of a single multi-segment graph. You can use NaN (as ``float('nan')``) for Y values where the function is undefined or the resonator is unstable. A single line containing NaN values will be broken into several segments and also become a multi-segment graph. Line labels "T" and "S" are treated additionally so that a special :ref:`graph format <app_settings_lines>` for tangential and sagittal planes gets applied.

For simple text output or debugging, use :doc:`custom_script`. For tabular data presentation, consider :doc:`custom_table`.

Element Locking
---------------

When your calculation varies element parameters (e.g., in a parameter sweep), you should lock the element before modifying it and unlock it afterward. This approach:

- Preserves the original parameter value for automatic restoration
- Disables UI updates during calculations for better performance
- Prevents recalculation of built-in functions while parameters change

.. code-block:: python

  elem.lock()
  try:
    for i in range(point_count):
      elem.set_param('R', value)
      # Perform calculations...
  finally:
      elem.unlock()

Always use a try/finally block to ensure the element is unlocked even if an error occurs.

Custom Library
--------------

You can save your code for later usage into a :ref:`custom code library <custom_func_lib>` using the :menuSelection:`Script --> Save to Custom Library` menu command. This allows you to build a collection of reusable calculation scripts for common tasks

Custom Parameters
-----------------

Custom parameters are arbitrary parameters that you can add to any element in addition to its predefined parameters. They are especially useful for custom table functions because they allow you to calculate or track properties that are not covered by the element's default parameters.

You can add custom parameters to elements through the element properties dialog, and then access them in your calculations using the :ref:`Element.param()<py_method_elem_param>` method. This provides flexibility to extend elements with project-specific data or properties specific to your custom calculations.

Example Projects
----------------

Open complete working examples ``custom_plot.rez`` and ``misalign.rez`` available via :menuSelection:`File --> Open Example` to see how custom plot functions work in practice and use it as a template for your own calculations. Use the "custom plot" filter in the Open Example dialog to see available examples for the subject.

.. seeAlso::

  - :doc:`py_api`
  - :doc:`plot_window`
  - :doc:`custom_funcs`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :ref:`Example - Custom Caustic Function <py_example_custom_caustic>`
