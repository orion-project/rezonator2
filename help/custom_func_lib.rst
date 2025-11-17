.. _custom_func_lib:

Custom Functions Library
=========================

:menuSelection:`Functions --> Custom Functions Library`

The Custom Functions Library provides centralized management for your reusable custom functions. It allows you to save, organize, and reuse custom function code across different schemas, building a personal collection of calculation tools tailored to your specific needs.

Custom functions are stored as Python files in the user profile directory. Each file is assigned a user-readable name that is extracted from the script's docstring. Choose descriptive, meaningful names for your scripts to make them easy to find and identify later.

For example, this function will be displayed as "Beam Waist Calculator" in the library:

.. code-block:: python

  """
  Beam Waist Calculator

  Calculates the beam waist position and size
  for a given cavity configuration.
  """

  def calculate():
    # Your code here
    pass

The library window displays all saved custom function files in a list with icons indicating their type:

+---------------+------------------------------------------------------------------------------+
|               |                                                                              |
+===============+==============================================================================+
| |icon_python| | Basic custom scripts for :doc:`text output <custom_script>`                  |
+---------------+------------------------------------------------------------------------------+
| |icon_table|  | Functions with ``columns()`` method for :doc:`tabular output <custom_table>` |
+---------------+------------------------------------------------------------------------------+
| |icon_plot|   | Functions with ``figure()`` method for :doc:`graphical output <custom_plot>` |
+---------------+------------------------------------------------------------------------------+

.. |icon_python| image:: ../img/toolbar/func_script.svg
.. |icon_table| image:: ../img/toolbar/func_table.svg
.. |icon_plot| image:: ../img/toolbar/func_plot.svg

Click any file in the list to view its code in the right panel. The code viewer is read-only by default, preventing unintended modifications.

Filter
------

Use the filter field at the top of the file list to quickly find custom functions. You can quickly activate the field by hitting the :guiLabel:`F3` hotkey. You can type multiple terms (minimum 2 characters per term) separated by spaces. Functions are shown only if their names contain all search terms (AND logic). Search is case-insensitive. For example, typing "beam plot" will show only functions with both "beam" and "plot" in their names.


Edit
----

For minor corrections and typo fixes, you can edit files directly in the window. Use the :guiLabel:`Edit` button on the toolbar or hit the :guiLabel:`F2` hotkey. Make your changes in the code editor and click :guiLabel:`Save` (:guiLabel:`Ctrl+S`) to save changes or :guiLabel:`Cancel` (:guiLabel:`Esc`) to discard them.

For regular development, you should add the function to a real schema via the :guiLabel:`Apply to Schema` command, modify and test it there to verify it works correctly. Then save it back to the library using the :menuSelection:`Save to Custom Library` menu command in the :menuSelection:`Script`, :menuSelection:`Table`, or :menuSelection:`Plot` menu, depending on the function type. This workflow ensures your scripts are properly tested with actual data before updating the library.

Create
------

To add a new custom function to the library, write a new :doc:`custom_script` or :doc:`custom_table` or :doc:`custom_plot`. Add a docstring with a human-readable name on the first line. Then use the :menuSelection:`Save to Custom Library` menu command in the :menuSelection:`Script`, :menuSelection:`Table`, or :menuSelection:`Plot` menu, depending on the function type.

If a function with the same name already exists in the library, you'll be asked whether to replace it. The library supposes that every custom functionality should be named uniquely. If one repeats a name, the one highly likely wants to reimplement the functionality.

Delete
------

To remove a custom function from the library, hit the :guiLabel:`Delete` button. Deleting a function from the library does not affect any schemas that use it. Existing schemas retain their own copies of the code.

Import/Export
-------------

Custom functions are stored as Python files in the user profile directory. Use the :guiLabel:`Export` and :guiLabel:`Import` buttons to share and backup. Importing files must be UTF-8 encoded Python files containing a docstring with a function name on the first line and the ``calculate()`` function.

.. seeAlso::

  - :doc:`functions`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :doc:`py_api`
