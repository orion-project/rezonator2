************
Introduction
************

|rezonator| is software for automation of routine calculations of various beam parameters in laser resonators and also in regular single-pass systems, e.g., pump paths.

The conventional ABCD ray matrix approach is used for calculation. Each optical element is described by using its ray matrix, and the whole optical system is described by means of the product of ray matrices of all elements composing the system.

When you use |rezonator|, you just have to build up an optical system from the proposed set of optical elements and assign desired values to parameters of all elements.

**Basic software possibilities:**

- :doc:`Elements catalog <catalog>`. The elements catalog contains a number of basic optical elements required for designing a laser resonator or a pumping system.

- :doc:`Layout <layout>`. Schema layout is the automatically formed visual representation of the optical system.

- :doc:`Simple <func_stabmap>` and :doc:`2D <func_stabmap_2d>` stability maps. Stability maps of a resonator can be calculated in two planes (tangential and sagittal) depending on the variation of one or two parameters.

- :doc:`Caustic <func_caustic>` and :doc:`Multicaustic <func_caustic_mr>`. Graphs of beam's radius and wavefront curvature radius can be plotted as along a single element so as along several or all elements.

- :doc:`Adjustment <adjust>`. The adjustment tool allows you to smoothly change a value of elements' parameters and to instantly see what influence it has on the operation of the schema (stability, beam shape).

**See chapters for details:**

:doc:`functions`, :doc:`utils`, :doc:`tools`
