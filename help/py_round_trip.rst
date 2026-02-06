.. index:: single: RoundTrip (Python)

Class: RoundTrip
================

:ref:`Round-trip <round_trip>` calculator for beam parameters and stability.

.. note::

  RoundTrip objects cannot be created directly; use the :ref:`schema.round_trip() <py_method_schema_round_trip>` function to obtain them.

Properties
----------

``ref`` (:doc:`Element <py_element>`, read-only)
    Reference element of the round-trip

.. _py_method_rt_plane:

``plane`` (int, read-only)
    :ref:`Work plane <work_plane>` (:ref:`Z.PLANE_T <py_const_plane_t>` or :ref:`Z.PLANE_S <py_const_plane_t>`)

``ior`` (float, read-only)
    Current index of refraction. Index of refraction is assigned automatically depending on what the reference element is. For example, if the element is an :ref:`interface <ElemNormalInterface>`, then the IOR will be taken from a :ref:`medium <ElemMediumRange>` element immediately following the interface.

``matrix_count`` (int, read-only)
    Number of matrices in the round-trip

``stabil_nor`` (float, read-only)
    :ref:`Stability parameter <stability_param>` (normal mode): `P = (A + D)/2`

``stabil_sqr`` (float, read-only)
    :ref:`Stability parameter <stability_param>` (squared mode): `P = 1 - ((A + D)/2)²`

Methods
-------

.. ##################################################################

.. index:: single: matrix (Python)
.. _py_method_rt_matrix:

``matrix(index=None)``
~~~~~~~~~~~~~~~~~~~~~~

Calculate and return the round-trip matrix at the reference point.

Arguments:

- ``index`` (int, optional) - Matrix index, or None for total round-trip matrix

Returns:

- :doc:`Matrix <py_matrix>` - The requested matrix

Raises:

- ``IndexError`` - If index is invalid

.. ##################################################################

.. index:: single: beam_radius (Python)
.. _py_method_rt_beam_radius:

``beam_radius()``
~~~~~~~~~~~~~~~~~

Calculate the round-trip matrix at the reference point and return the :ref:`beam radius <abcd_beam_size>` computed from the matrix components.

Returns:

- float - Beam radius in meters

.. ##################################################################

.. index:: single: front_radius (Python)
.. _py_method_rt_front_radius:

``front_radius()``
~~~~~~~~~~~~~~~~~~

Calculate the round-trip matrix at the reference point and return the :ref:`wavefront radius of curvature <abcd_beam_roc>` computed from the matrix components.

Returns:

- float - Wavefront radius in meters

.. ##################################################################

.. index:: single: half_angle (Python)
.. _py_method_rt_half_angle:

``half_angle()``
~~~~~~~~~~~~~~~~

Calculate the round-trip matrix at the reference point and return the :ref:`half of the divergence angle in the far field <abcd_beam_angle>` computed from the matrix components.


Returns:

- float - Half angle in radians

.. ##################################################################

.. index:: single: beam (Python)
.. _py_method_rt_beam:

``beam()``
~~~~~~~~~~

Calculate the round-trip matrix at the reference point and return :ref:`all beam properties <abcd_beam>` computed from the matrix components.

Returns:

- tuple(float, float, float) - (beam_radius, front_radius, half_angle) in m, m, rad

.. ##################################################################

.. note::

  :ref:`Round-trip matrix <round_trip>` computation is automatically performed before each beam property calculation (e.g., ``beam_radius()``). This guaranties that the actual elements' parameters are taken into account. For multiple queries on the same property, store the result into a local variable rather than recalculating.

.. ##################################################################

.. index:: single: elem (Python)
.. _py_method_rt_elem:

``elem(index)``
~~~~~~~~~~~~~~~

Get the element owning the specified matrix.

Arguments:

- ``index`` (int) - Matrix index

Returns:

- :doc:`Element <py_element>` - The element owning this matrix

Raises:

- ``IndexError`` - If index is invalid

.. ##################################################################

Example
-------

.. code-block:: python

    import rezonator as Z
    import schema

    def calculate():
        ref_elem = schema.elem(4)

        # Set offset inside element
        if ref_elem.length is not None:
            ref_elem.offset = ref_elem.length / 2.0

        # Create round trip calculators for both planes
        rt_t = schema.round_trip(ref=ref_elem, inside=True, plane=Z.PLANE_T)
        rt_s = schema.round_trip(ref=ref_elem, inside=True, plane=Z.PLANE_S)

        # Get matrices
        Z.print('Tangential matrix:', rt_t.matrix())
        Z.print('Sagittal matrix:', rt_s.matrix())

        # Get stability parameters
        Z.print('Stability (T, normal):', rt_t.stabil_nor)
        Z.print('Stability (S, normal):', rt_s.stabil_nor)
        Z.print('Stability (T, squared):', rt_t.stabil_sqr)
        Z.print('Stability (S, squared):', rt_s.stabil_sqr)

        # Get beam parameters
        w_t = rt_t.beam_radius()
        w_s = rt_s.beam_radius()
        Z.print(f'Beam size: {w_t*1e3:.3f} × {w_s*1e3:.3f} mm')

.. seeAlso::

  - :doc:`py_api`
  - :doc:`py_element`
  - :doc:`py_matrix`
  - :ref:`Example - Round Trip and Beam Properties <py_example_round_trip>`
  - :ref:`Example - Custom Caustic Function <py_example_custom_caustic>`
