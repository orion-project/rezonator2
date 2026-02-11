.. _func_misalign:

.. |y_eff| replace:: `dy`\ :sub:`eff`
.. |V_eff| replace:: `dV`\ :sub:`eff`

Misalignments
=============

Calculation of misalignment is done according to the book "Lasers" by A.E. Siegman, see chapter 15.4 "Ray optics with misaligned elements". The same is described in A. Gerrard and J. M. Burch, "Introduction to Matrix Methods in Optics", Appendix II.

*Misalignment* of an optical element is its displacement to some small distance `dy` and tilt to a small angle `dV` relative to the ideal optical axis of the system.

  .. image:: img/misalign.png

Misalignments of individual elements are set via :ref:`custom parameters <custom_params>`. For this particular script, it's supposed that those parameters are `dYt` and `dVt` for axial and angular misalignments in the tangential plane, and `dYs` and `dVs` are the same for the sagittal plane, so we call them just `dy` and `dV` in following formulas.

Overall Misalignment
----------------------

Calculation of :ref:`misalignments <misalign>` is done via extended 3×3 ABCDEF ray transfer matrices where additional `E` and `F` elements represent the element's displacement and tilt, respectively.

  .. tex:
    \begin{bmatrix}
    A & B & E\\
    C & D & F\\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_matrix_ex.png

Where the `E` and `F` components of an individual element's matrix are calculated from element's ABCD matrix components and misalignment parameters `dy` and `dV` as

  .. image:: img/misalign_eq_12.png

Then the overall misalignment vector `E`\ :sub:`0` and `F`\ :sub:`0` which affects the beam displacement and slope after transition through the misaligned system, is calculated via the :ref:`production <round_trip>` of such extended matrices:

  .. image:: img/misalign_eq_16.png

If not involving 3×3 matrices, the overall misalignment of an array of elements having ABCD matrices `M`\ :sub:`1` ... `M`\ :sub:`n` and individual misalignments [`E`\ :sub:`1`, `F`\ :sub:`1`] ... [`E`\ :sub:`n`, `F`\ :sub:`n`] can be expressed as:

  .. image:: img/misalign_eq_17.png

Effective Optical Axis of SP System
-----------------------------------

Any system with misaligned elements can be converted into an effectively aligned system by physically translating the whole system downward from the reference optical axis by the distance |y_eff| and then physically rotating it toward the system axis by the angle |V_eff|, with the center of rotation at the input plane, by the angle

  .. image:: img/misalign_eq_18.png

where `L` is the overall system length and `A`, `B`, `C`, `D`, `E`, `F` are matrix elements of the whole system.

Effective Optical Axis of Resonator
-----------------------------------

Position of the effective optical axis is defined as a displacement |y_eff| and slope |V_eff| (relative to the reference optical axis of the system) of some ray that comes back to the original point after the full pass through the resonator (round-trip). In other words, the position of the effective optical axis is a self-conformed misalignment:

  .. image:: img/misalign_eq_20.png

where `A`, `B`, `C`, `D`, `E`, `F` are matrix elements of the entire system at some reference plane.

.. seeAlso::

  - :doc:`py_api`
  - :doc:`custom_script`
  - :doc:`round_trip_matrix`
  - :doc:`misalign`
