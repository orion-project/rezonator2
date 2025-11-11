.. index:: single: rezonator (Python)

Module: rezonator
=================

The ``rezonator`` module (aliased as ``Z`` for brevity) provides global constants, utility functions, and classes for matrix and beam calculations.

Constants
---------

.. ###################################################################

.. index:: single: C (Python)

``Z.C``
  Speed of light in m/s

.. ###################################################################

.. index:: single: PI (Python)

``Z.PI``
  Pi constant

.. ###################################################################

.. index:: single: PLANE_T (Python)
.. _py_const_plane_t:

``Z.PLANE_T``
  :ref:`Tangential <working_planes>` plane

.. ###################################################################

.. index:: single: PLANE_S (Python)
.. _py_const_plane_s:

``Z.PLANE_S``
  :ref:`Sagittal <working_planes>` plane

.. ###################################################################

.. index:: single: DIM_NONE (Python)

``Z.DIM_NONE``
  Dimensionless.

.. ###################################################################

.. index:: single: DIM_LINEAR (Python)

``Z.DIM_LINEAR``
  Linear dimension (length)

.. ###################################################################

.. index:: single: DIM_ANGULAR (Python)

``Z.DIM_ANGULAR``
  Angular dimension

.. ###################################################################

Functions
---------

.. ###################################################################

.. index:: single: print (Python)

``print(*args, spaced=True)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Print messages to the function output window.

Parameters:

- ``*args`` - Values to print (strings, numbers, or objects with ``__repr__``). When an argument is a number, then it is formatted according to :ref:`application settings <app_settings_format_precision>`. To get unformatted output, use the Python's built-in function ``str`` to convert a number into string ignoring application settings.
- ``spaced`` (bool, optional) - If True (default), separate arguments with spaces

Example:

.. code-block:: python

    import rezonator as Z
    Z.print('Speed of light:', Z.C, 'm/s')  # Output: Speed of light: 2.998e+08 m/s
    Z.print('Speed of light:', str(Z.C), 'm/s') # Output: Speed of light: 299792458.0 m/s
    Z.print('L=', elem.length, spaced=False)  # Output: L=0.1

.. ###################################################################

.. index:: single: format (Python)

``format(value)``
~~~~~~~~~~~~~~~~~

Format a numeric value into a user-friendly display string respecting the :ref:`application settings <app_settings_format_precision>`.

Parameters:

- ``value`` (float) - The numeric value to format

Returns:

- str - Formatted string representation

Example:

.. code-block:: python

    import rezonator as Z
    Z.print('Length:', Z.format(0.00123))  # Output: "1.23e-3"

.. ###################################################################

.. index:: single: version (Python)

``version()``
~~~~~~~~~~~~~

Get the application version string, e.g., "2.1.2".

.. ###################################################################

.. index:: single: plane_str (Python)

``plane_str(plane)``
~~~~~~~~~~~~~~~~~~~~

Convert :ref:`work plane <work_plane>` constant to string name.

Parameters:

- ``plane`` (int) - Plane constant (``Z.PLANE_T`` or ``Z.PLANE_S``)

Returns:

- str - Plane name ('T' or 'S')

Example:

.. code-block:: python

    import rezonator as Z
    name = Z.plane_str(Z.PLANE_T)  # Returns 'T'

.. ###################################################################

.. seeAlso::

  - :doc:`py_api`
  - :doc:`py_schema`
