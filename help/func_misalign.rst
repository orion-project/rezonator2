Misalignments
=============

Calculation of misalignment is done according to the book "Lasers" by A.E. Siegman, see chapter 15.4 "Ray optics with misaligned elements". The same is described in A. Gerrard and J. M. Burch, "Introduction to Matrix Methods in Optics", Appendix II.

*Misalignment* of an optical element is its displacement to some small distance `Δy` and tilt to a small angle `ΔV` relative to the ideal optical axis of the system.

  .. image:: img/misalign.png

Misalignments of individual elements are set via :ref:`custom parameters <custom_params>`. For this particular script, it's supposed that those parameters are `dYt` and `dVt` for axial and angular misalignments in the tangential plane, and `dYs` and `dVs` are the same for the sagittal plane.

Effective Misalignment
----------------------

Calculation of misalignments is done via extended 3×3 ABCDEF ray transfer matrices where additional `E` and `F` elements represent the element's displacement and tilt, respectively.

  .. tex:
    \begin{bmatrix}
    A & B & E\\
    C & D & F\\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_matrix_ex.png

Then *effective misalignment* (or overall misalignment), which is beam displacement and slope after transition through the misaligned system, is calculated via a :ref:`production <round_trip>` of such extended matrices:

  .. tex:
    \begin{bmatrix}
    A & B & \Delta y_{eff}\\
    C & D & \Delta V_{eff}\\
    0 & 0 & 1
    \end{bmatrix}
    =
    \begin{bmatrix}
    A_n & B_n & \Delta y_n\\
    C_n & D_n & \Delta V_n\\
    0 & 0 & 1
    \end{bmatrix}
    \times
    ...
    \times
    \begin{bmatrix}
    A_2 & B_2 & \Delta y_2\\
    C_2 & D_2 & \Delta V_2\\
    0 & 0 & 1
    \end{bmatrix}
    \times
    \begin{bmatrix}
    A_1 & B_1 & \Delta y_1\\
    C_1 & D_1 & \Delta V_1\\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_matrix_eff.png

If not involving 3×3 matrices, the overall misalignment of an array of elements having ABCD matrices `M`\ :sub:`1` ... `M`\ :sub:`n` and individual misalignments [`Δy`\ :sub:`1`, `ΔV`\ :sub:`1`] ... [`Δy`\ :sub:`n`, `ΔV`\ :sub:`n`] can be expressed as:

  .. image:: img/misalign_1.png

or more briefly

  .. image:: img/misalign_2.png

Effective Optical Axis of SP System
-----------------------------------

According to Siegman, any system with misaligned elements can be converted into an effectively aligned system by physically translating the whole system downward from the reference optical axis by the distance

  .. tex:
    \Delta y_0 = \cfrac{
    (1 - D) E - (L - B) F
    }{
    (1 - A)(1 - D) + (L - B) C
    }
  .. image:: img/misalign_eff_sp_y.png

and then physically rotating it toward the system axis, with the center of rotation at the input plane, by the angle

  .. tex:
    \Delta V_0 = \cfrac{
    C E + (1 - A) F
    }{
    (1 - A)(1 - D) + (L - B) C
    }
  .. image:: img/misalign_eff_sp_v.png

where `L` is the overall system length and `A`, `B`, `C`, `D`, `E`, `F` are matrix elements of the whole system, so that `E ≡ Δy`\ :sub:`eff` and `F ≡ ΔV`\ :sub:`eff` are effective axial and angular misalignment, respectively.

Effective Optical Axis of Resonator
-----------------------------------

Position of the effective optical axis is defined as a displacement `Δy`\ :sub:`0` and slope `ΔV`\ :sub:`0` (relative to the reference optical axis of the system) of some ray that comes back to the original point after the full pass through the resonator (round-trip). In other words, the position of the effective optical axis is a self-conformed misalignment:

  .. tex:
    \Delta y_0 = \cfrac{(1 - D) E - B F}{2 - A - D},
    \quad
    \Delta V_0 = \cfrac{CE + (1 - A)F}{2 - A - D}
  .. image:: img/misalign_eff_sw.png

where `A`, `B`, `C`, `D`, `E`, `F` are matrix elements of the entire system at some reference plane, so that `E ≡ Δy`\ :sub:`eff` and `F ≡ ΔV`\ :sub:`eff` are effective axial and angular misalignment, respectively.

.. seeAlso::

  - :doc:`py_api`
  - :doc:`custom_script`
  - :doc:`round_trip_matrix`
