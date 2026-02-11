Glossary
========
.. ##################################################################

.. _axial_len:
.. _axial_length:
.. index:: single: axial length

Axial length
------------

Axial length is a geometrical distance that a beam travels inside an element. For most cases it is the same as the element’s Length parameter values. But there are some elements (e.g., :doc:`matrix/ElemTiltedPlate`) for which they are different because their length is measured from edge to edge normally, but the beam passes at an angle to that direction and covers a larger distance.

    .. image:: img/axial_len.png

.. ##################################################################

.. _propagation_dir:
.. index:: single: propagation direction

Propagation Direction
---------------------

|rezonator| supposes a beam is propagating from left to right in the schema. For :ref:`single-pass <schema_kind_sp>` schema, it is as if a light source would be located just before the leftmost element of the schema and is directed to the right side.

    .. image:: img/propagation_sp.png

For :ref:`resonators <schema_kind_rr>`, it is just the chosen primary direction. In :ref:`standing-wave resonator <schema_kind_sw>`, the beam reflects from the rightmost element and does round-trip returning to the leftmost one.

    .. image:: img/propagation_res.png

When the :doc:`round-trip matrix <round_trip_matrix>` is calculated, elements' matrices are multiplied starting from some reference element and in the direction opposite to beam propagation, as the ray-matrix approach supposes.

.. ##################################################################

.. _ray_vector:
.. index:: single: ray vector

Ray Vector
----------

Ray vector represents a geometric beam approximation when a beam described by two parameters - a distance from the optical axis `r` and an angle between the beam and the optical axis `V`.

    .. image:: img/ray_vector.png

When a beam traverses an optical system, parameters of the output beam can be expresses from parameters of the input beam and system's ray matrix `M = [A, B; C, D]`.

    .. image:: img/ray_vector_1.png

|rezonator| can compute ray vector propagation in :ref:`single-pass <schema_kind_sp>` schemas, see :ref:`Pump mode: Ray vector <pump_mode_vector>`.

.. ##################################################################

.. _work_plane:
.. _work_planes:
.. _working_planes:
.. index:: single: tangential plane
.. index:: single: sagittal plane
.. index:: single: T-plane
.. index:: single: S-plane

.. ##################################################################

.. _reduced_slope:
.. index:: single: reduced slope

Reduced Slope
-------------

When defining a :ref:`ray vector <ray_vector>`, some sources use so-called “reduced slope” instead of the geometrical angle between the ray and the optical axis. Then the angle is multiplied by the index of refraction of the medium where the ray is measured: `r' = n * dr/dz`, where `z` is the distance along the optical axis. This approach can simplify some matrices; for example, a :ref:`normal interface <ElemNormalInterface>` will have the unity matrix. But this only makes a difference when input and output reference planes are in different media (for interface elements). Also, this approach seems not very practical when displaying results of calculations —- to display ray angles, we always must remember the medium IOR. So |rezonator| uses geometrical angles `V ~ tg V = dr/dz` for ray vectors, and all element matrices are defined accordingly.

.. ##################################################################

.. _stability_param:
.. index:: single: stability parameter

Stability Parameter
-------------------

Stability parameter is a value showing if a system can support stable laser operation. It can be calculated in two ways:

* `P = (A + D) / 2`
* `P = 1 — ((A + D) / 2)` :sup:`2`

where A and D are system :ref:`round-trip matrix <round_trip>` components. In the first case the system is stable when the parameter value is in range from -1 to 1. In the second case the system is stable when the parameter value is in the range from 0 to 1.

.. ##################################################################

Working Planes
--------------

**Tangential Plane, T-plane**

The tangential plane is the plane containing all the resonator's elements (the plane of an optical stand). Beams are refracted or reflected in this plane.

**Sagittal Plane, S-plane**

The sagittal plane is the plane containing the element's optical axis and perpendicular to the tangential plane.

    .. image:: img/planes_ts.png
