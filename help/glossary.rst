Glossary
========

.. _propagation_dir:
.. index:: single: propagation direction

Propagation Direction
---------------------

|rezonator| supposes a beam propagating from left to right in the schema. For :ref:`single-pass <schema_kind_sp>` schema, it is as if a light source would be located just before the leftmost element of the schema and is directed to the right side.

    .. image:: img/propagation_sp.png

For :ref:`resonators <schema_kind_rr>`, it is just the chosen primary direction. In :ref:`standing-wave resonator <schema_kind_sw>`, the beam reflects from the rightmost element and does round-trip returning back to the leftmost one. 

    .. image:: img/propagation_res.png

When the :doc:`round-trip matrix <round_trip_matrix>` is calculated, elements' matrices are multiplied starting from some reference element and in the direction opposite to beam propagation, as the ray-matrix approach supposes.



.. _working_planes:
.. index:: single: tangential plane
.. index:: single: sagittal plane
.. index:: single: T-plane
.. index:: single: S-plane

Working Planes
--------------

**Tangential Plane, T-plane**

Tangential plane is the plane containing all the resonator's elements (the plane of an optical stand). Beams are refracted or reflected in this plane.

**Sagittal Plane, S-plane**

Sagittal plane is the plane containing element's optical axis and perpendicular to the tangential plane. 

    .. image:: img/planes_ts.png
