.. _custom_funcs:

Custom Functions
================

Custom functions provide a powerful way to extend |rezonator|'s capabilities by writing your own Python code to analyze optical schemas, perform specialized calculations, and visualize results in ways that suit your specific needs.

There are three types of custom functions, each designed for different output formats:

- :doc:`Custom Script <custom_script>` - For general-purpose calculations with text output to a log panel
- :doc:`Custom Table <custom_table>` - For calculations presented in structured tabular format
- :doc:`Custom Plot <custom_plot>` - For calculations visualized as graphs and plots

All custom functions use the same :doc:`Python API <py_api>` to interact with your optical schema, access element properties, and perform calculations.

Overview of Custom Function Types
----------------------------------

Custom Script
~~~~~~~~~~~~~

:menuselection:`Functions --> New Custom Script Window`

The :doc:`custom_script` function is the simplest form of custom function, designed for arbitrary calculations with text-based output. It's ideal for:

- Quick calculations and debugging
- Generating custom reports
- Performing one-off analyses
- Prototyping more complex functions

Results are displayed in a log panel at the bottom of the script window using the :ref:`rezonator.print() <py_module_z_print>` function.

  .. .. note::
  ..   Script code is stored only in the editor window and will be lost when the window is closed. Save your scripts to the :ref:`custom code library <custom_func_lib>` for reuse.

Custom Table
~~~~~~~~~~~~

:menuselection:`Functions --> Create Custom Table Function`

The :doc:`custom_table` function presents calculation results in a structured :doc:`table format <table_window>`. It's ideal for:

- Comparing properties across multiple elements
- Calculating values at different positions along the optical path
- Displaying results with automatic unit conversion
- Presenting data that benefits from tabular organization

The function automatically calls your code for each element and position, building a comprehensive table of results.

  .. .. note::
  ..   Table code is stored in the table window and persists when you close the code editor. The function is lost only when you close the table window itself.

Custom Plot
~~~~~~~~~~~

:menuselection:`Functions --> Create Custom Plot Function`

The :doc:`custom_plot` function visualizes calculation results as graphs in a :doc:`plot window <plot_window>`. It's ideal for:

- Visualizing dependencies and trends
- Parameter sweep analyses
- Beam propagation visualization
- Stability diagrams and other graphical analyses

The function generates data points that are automatically plotted with full support for multiple graph lines, unit conversion, and interactive plot controls.

  .. .. note::
  ..   Plot code is stored in the plot window and persists when you close the code editor. The function is lost only when you close the plot window itself.

Common Features
---------------

All custom function types share several common capabilities:

Python API Access
~~~~~~~~~~~~~~~~~

All custom functions use the same comprehensive :doc:`py_api` which provides:

- Access to schema elements via :doc:`Element <py_element>` objects
- Round-trip calculations via :doc:`RoundTrip <py_round_trip>` objects
- Matrix operations via :doc:`Matrix <py_matrix>` and :doc:`Matrix3 <py_matrix3>` objects
- Schema-level access via :doc:`Schema <py_schema>` object
- Utility functions in the global :doc:`rezonator module <py_global>`

Custom Parameters
~~~~~~~~~~~~~~~~~

:ref:`Custom parameters <custom_params>` allow you to extend any element with additional user-defined parameters beyond its built-in parameters. This is especially useful for custom functions because you can:

- Store intermediate calculation results
- Track properties specific to your analysis
- Add element-specific configuration data
- Create parameters for properties not covered by default

Custom parameters are added through the :ref:`Element Properties dialog <elem_props>` and accessed in your code using the :ref:`Element.param() <py_method_elem_param>` method.

Code Library
~~~~~~~~~~~~

All custom functions can be saved to a :ref:`custom code library <custom_func_lib>` using :menuselection:`Script --> Save to Custom Library`. This allows you to:

- Build a collection of reusable calculation scripts
- Share analysis code across different schemas
- Maintain a library of common calculations
- Quickly create new functions from templates

The library stores both the code and metadata (function name, description, creation date) for easy organization and retrieval.

SI Units
~~~~~~~~

All calculations in |rezonator| use SI units internally (meters, radians, etc.):

- Parameter values are always provided in SI units
- For **custom scripts**: You should manually format output for readability (e.g., convert meters to millimeters)
- For **custom tables and plots**: Return values in SI units; the UI automatically handles unit conversion based on dimension types

Choosing the Right Function Type
---------------------------------

Consider the following when selecting a custom function type:

Use **Custom Script** when you need:
  - Simple text-based output
  - Quick calculations or debugging
  - Custom reports or summaries
  - Maximum flexibility without a predefined output format

Use **Custom Table** when you need:
  - Structured presentation of multiple values
  - Comparison across elements or positions
  - Automatic unit conversion in the UI
  - Organized tabular data display

Use **Custom Plot** when you need:
  - Graphical visualization of results
  - Trend analysis or parameter sweeps
  - Interactive plots with zoom and pan
  - Multiple graph lines on the same axes

Getting Started
---------------

1. **Learn the Python API**: Review the :doc:`py_api` documentation to understand available objects and methods

2. **Study Examples**: Open example projects via :menuselection:`File --> Open Example`:

  - ``custom_func_simple.rez`` - Generic coding examples
  - ``custom_func_aper_ratio.rez`` - Simplest custom table example
  - ``custom_plot.rez`` - Simplest custom plot example
  - ``misalign.rez`` - Combined custom table and plot example

3. **Start Simple**: Begin with a :doc:`custom_script` to understand the API, then progress to tables and plots

4. **Use Custom Parameters**: Extend elements with :ref:`custom parameters <custom_params>` when you need to store additional data

5. **Build Your Library**: Save successful functions to the :ref:`custom code library <custom_func_lib>` for reuse

.. seeAlso::

  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :doc:`custom_params`
  - :doc:`custom_func_lib`
  - :doc:`py_api`
  - :doc:`py_examples`
