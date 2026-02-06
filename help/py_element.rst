.. index:: single: Element (Python)

Class: Element
==============

Represents an optical :doc:`element <elements>` in the schema.

.. note::

  Element objects cannot be created directly; use the :ref:`schema.elem() <py_method_schema_elem>` function to obtain them.

Properties
----------

``label`` (str, read-only)
  Element's :ref:`label <elem_props_label>`.

``index`` (int, read-only)
  Element's 1-based index in schema (or None if not found). Unlike regular Python lists, elements in schema are indexed starting from 1 to be consistent with how they are numbered in the :doc:`schema_elems`.

``length`` (float, read-only)
    Length parameter in meters (None if element has no length).

``axial_length`` (float, read-only)
    :ref:`Axial length <axial_length>` in meters (None if element has no length).

``optical_path`` (float, read-only)
    Optical path length in meters (None if element has no length).

``ior`` (float, read-only)
    Index of refraction (None if not applicable).

``offset`` (float, read/write)
    Offset inside the element in meters (None if element has no length). The offset value is used for elements having length when the round-trip is :ref:`calculated <calc_round_trip_subrange>` with ``inside=True``. See :ref:`schema.round_trip() <py_method_schema_round_trip>`.

``prev`` (Element, read-only)
    Previous element respecting :ref:`round-trip <round_trip>` rules. The property also ignores :ref:`disabled <elem_props_lock>` elements because they do not participate in the round-trip.

``next`` (Element, read-only)
    Next element respecting :ref:`round-trip <round_trip>` rules. The property also ignores :ref:`disabled <elem_props_lock>` elements because they do not participate in the round-trip.

``disabled`` (bool, read-only)
    Whether the element is :ref:`disabled <elem_props_lock>`.

.. #######################################################################

Methods
-------

.. index:: single: param (Python)
.. _py_method_elem_param:

``param(label, default=None)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get element parameter value by label. This method gives access to both predefined and :ref:`custom <custom_params>` parameters of an element.

Arguments:

- ``label`` (str) - Parameter label
- ``default`` (float, optional) - Default value if parameter not found

Returns:

- float - Parameter value in SI units, or None/default if not found

Example:

.. code-block:: python

    elem = schema.elem('F1')
    f = elem.param('F')  # Get focal length
    r = elem.param('dYt', 0.0)  # Get axial misalignment, default to 0 if not found

.. #######################################################################

.. index:: single: set_param (Python)
.. _py_method_elem_set_param:

``set_param(label, value)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set element parameter value by label.

Arguments:

- ``label`` (str) - Parameter label
- ``value`` (float) - Parameter value in SI units

Raises:

- ``KeyError`` - If parameter is not found
- ``ValueError`` - If value is invalid

Example:

.. code-block:: python

    elem = schema.elem('L1')
    elem.set_param('F', 0.1)  # Set focal length to 100 mm

.. #######################################################################

.. index:: single: matrix (Python)
.. _py_method_elem_matrix:

``matrix(plane)``
~~~~~~~~~~~~~~~~~

Return element's matrix in the given plane, or in T plane if the argument is not provided.

Arguments:

- ``plane`` - :ref:`Work plane <work_plane>` ('T', 'S', :ref:`Z.PLANE_T <py_const_plane_t>`, or :ref:`Z.PLANE_S <py_const_plane_s>`)

Raises:

- ``ValueError`` - If invalid plane name


.. #######################################################################

.. index:: single: lock (Python)
.. _py_method_elem_lock:

``lock()``
~~~~~~~~~~

Disable element events and backup parameter values. Used when making temporary changes.

.. #######################################################################

.. index:: single: unlock (Python)
.. _py_method_elem_unlock:

``unlock()``
~~~~~~~~~~~~

Enable element events and restore backed up parameter values.

.. note::

  Use ``Element.lock()`` and ``Element.unlock()`` methods when making temporary parameter changes (e.g., during :doc:`plot function <custom_plot>` calculation) to ensure proper restoration and also to preventing the UI from unnecessary updates.

Example:

.. code-block:: python

  elem = schema.elem('Cr')
  elem.lock()
  try:
    # Make temporary changes:
    elem.length = elem.length * 1.1

    # Calculate something...
  finally:
    # Restore original values
    elem.unlock()

.. seeAlso::

  - :doc:`py_api`
  - :doc:`elem_props`
  - :ref:`Example - Basic Element Manipulation <py_example_basic_elem_manipulation>`
