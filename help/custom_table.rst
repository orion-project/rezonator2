Custom Table Function
=====================

:menuSelection:`Functions --> Create Custom Table Function`

The Custom Table Function allows you to perform arbitrary user calculations using the provided :doc:`py_api` and output the results into a :doc:`table view <table_window>` rather than in text form like the simpler :doc:`custom_script` function does. This powerful feature enables you to analyze your optical schema, perform custom computations, and present results in a structured tabular format, making it ideal for comparing values across multiple elements or positions.

For custom functions, the table window shows an additional command :menuSelection:`Table --> Show Code` and the respective button with the Python icon on the toolbar. The command opens a code editor window to change the code of your script. This window can be safely closed because the code is actually stored in the table window, not in the code window. However, if you close the table window itself, then the function code *will be lost*.

Code Module
-----------

A custom function is a user-defined Python module. The first line of the module docstring will be used as a human-readable function name and displayed in the function window headers. This allows you to give your function a meaningful title.

``columns()``
~~~~~~~~~~~~~

The code module must provide a ``columns()`` function that defines the structure and properties of the table columns. This function is called before each recalculation, which allows you to dynamically change table structure (such as column titles based on selected elements or parameters) without reopening the window.

.. code-block:: python

  def columns():
    return [
      {
        'label': 'W',
        'title': 'Beam radius',
        'dim': Z.DIM_LINEAR
      },
      {
        'label': 'Ratio',
        'title': 'Aperture ratio',
        'dim': Z.DIM_NONE
      }
    ]

Each column definition is a dictionary with the following keys:

- ``label`` - A short name of the calculated value; it is displayed in the column title and also used for extracting values from a result dict returned by the ``calculate()`` function
- ``title`` - Full descriptive title that is displayed in menus of the table window
- ``dim`` - Dimension type (:ref:`Z.DIM_LINEAR <py_const_dim_linear>`, :ref:`Z.DIM_ANGULAR <py_const_dim_angular>`, :ref:`Z.DIM_NONE <py_const_dim_none>`) used when choosing a measurement unit for the column

``calculate()``
~~~~~~~~~~~~~~~

The code must also contain a ``calculate()`` function that performs the actual calculations. For table functions, this function is called multiple times - at least twice for each element (for T and S planes separately), and possibly :doc:`several times <table_symbols>` for the same element depending on the :ref:`options <table_opers_calc_at_medium_ends>` selected in the table window.

.. code-block:: python

  def calculate(elem, pos, rt):
    # Perform calculations for the given element and position
    if pos == POS_RIGHT:
      w = rt.beam_radius()
      return {'W': w}
    return None

The function receives three arguments:

- ``elem`` - The current :doc:`Element <py_element>` object being processed
- ``pos`` - Position indicator with possible values:

  - ``POS_LEFT`` - Left side of the element (before it)
  - ``POS_BEG`` - Beginning inside the element
  - ``POS_MID`` - Middle inside the element
  - ``POS_END`` - End inside the element
  - ``POS_RIGHT`` - Right side of the element (after it)

- ``rt`` - Precalculated :doc:`RoundTrip <py_round_trip>` object for the current position. The :ref:`RoundTrip.plane <py_method_rt_plane>` property shows for which workplane the function is called.

The function should return:

- A dictionary with keys matching the column labels defined in ``columns()``, or
- ``None`` if no data should be displayed for this element/position combination

Output
~~~~~~

All calculations in |rezonator| are performed in SI units (meters, radians, etc.). All parameter values provided to scripts are in SI units as well. For custom table functions, you should return calculated values in SI units as well without any conversion. Because the table window has built-in capability to choose and display :ref:`units <table_opers_units>` according to the column's dimension type (``dim`` property).

The results from the ``calculate()`` function are automatically formatted and displayed in a :doc:`table_window`. Each row in the table corresponds to one element or position, and each column shows the calculated values as defined in your ``columns()`` function.

For simple text output or debugging, use :doc:`custom_script` instead. For graphical visualization, consider :doc:`custom_plot`.

Custom Library
--------------

You can save your code for later usage into a :ref:`custom code library <custom_func_lib>` using the :menuSelection:`Script --> Save to Custom Library` menu command. This allows you to build a collection of reusable calculation scripts for common tasks.

Custom Parameters
-----------------

Custom parameters are arbitrary parameters that you can add to any element in addition to its predefined parameters. They are especially useful for custom table functions because they allow you to calculate or track properties that are not covered by the element's default parameters.

You can add custom parameters to elements through the element properties dialog, and then access them in your calculations using the :ref:`Element.param()<py_method_elem_param>` method. This provides flexibility to extend elements with project-specific data or properties specific to your custom calculations.

Example Projects
----------------

Open complete working examples ``custom_func_aper_ratio.rez`` and ``misalign.rez`` available via :menuSelection:`File --> Open Example` to see how custom table functions work in practice and use it as a template for your own calculations. Use the "custom table" filter in the Open Example dialog to see available examples for the subject.

.. seeAlso::

  - :doc:`py_api`
  - :doc:`table_window`
  - :doc:`custom_script`
  - :doc:`custom_plot`
  - :ref:`Example - Aperture ratio calculation <py_example_aper_ratio>`
