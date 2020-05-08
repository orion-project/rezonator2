Glossary
========

.. _propagation_dir:
.. index:: single: propagation direction

Propagation Direction
---------------------

|rezonator| supposes a beam is propagating from left to right in the schema. For :ref:`single-pass <schema_kind_sp>` schema, it is as if a light source would be located just before the leftmost element of the schema and is directed to the right side.

    .. image:: img/propagation_sp.png

For :ref:`resonators <schema_kind_rr>`, it is just the chosen primary direction. In :ref:`standing-wave resonator <schema_kind_sw>`, the beam reflects from the rightmost element and does round-trip returning to the leftmost one. 

    .. image:: img/propagation_res.png

When the :doc:`round-trip matrix <round_trip_matrix>` is calculated, elements' matrices are multiplied starting from some reference element and in the direction opposite to beam propagation, as the ray-matrix approach supposes.



.. _ray_vector:
.. index:: single: ray vector

Ray Vector
----------

Ray vector represents a geometric beam approximation when a beam described by two parameters - a distance from the optical axis `r` and an angle between the beam and the optical axis `V`.

    .. image:: img/ray_vector.png

When a beam traverses an optical system, parameters of the output beam can be expresses from parameters of the input beam and system's ray matrix `M = [A, B; C, D]`.

    .. image:: img/ray_vector_1.png

|rezonator| can compute ray vector propagation in :ref:`single-pass <schema_kind_sp>` schemas, see :ref:`Pump mode: Ray vector <pump_mode_vector>`.

.. _working_planes:
.. index:: single: tangential plane
.. index:: single: sagittal plane
.. index:: single: T-plane
.. index:: single: S-plane

Working Planes
--------------

**Tangential Plane, T-plane**

The tangential plane is the plane containing all the resonator's elements (the plane of an optical stand). Beams are refracted or reflected in this plane.

**Sagittal Plane, S-plane**

The sagittal plane is the plane containing the element's optical axis and perpendicular to the tangential plane. 

    .. image:: img/planes_ts.png
