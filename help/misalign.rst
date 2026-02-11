.. _misalign:

Calculation of Misalignments
============================

.. |r1| replace:: `r`\ :sub:`1`
.. |r2| replace:: `r`\ :sub:`2`
.. |e1| replace:: `e`\ :sub:`1`
.. |e2| replace:: `e`\ :sub:`2`
.. |m1| replace:: `m`\ :sub:`1`
.. |m2| replace:: `m`\ :sub:`2`
.. |dy1| replace:: `dy`\ :sub:`1`
.. |y_eff| replace:: `dy`\ :sub:`eff`
.. |V_eff| replace:: `dV`\ :sub:`eff`

Suppose we have an element whose axis is displaced from the optical system axis (the reference axis) by |dy1| and rotated by `dV`. We are interested in how this displacement and rotation (the misalignment) affects a ray passing through such element or a collection of elements. We measure rays with respect to the reference optical axis.

  .. image:: img/misalign_calc.png

Derivation of ABCDEF Ray Matrices
---------------------------------

Lets introduce the follow symbols.

Input (|r1|) and output (|r2|) ray vectors with respect to the Reference axis:

  .. tex:
    r_1 = \begin{bmatrix} y_{1R} \\ V_{1R} \end{bmatrix}
    \qquad
    r_2 = \begin{bmatrix} y_{2R} \\ V_{2R} \end{bmatrix}
  .. image:: img/misalign_eq_01.png

Input (|e1|) and output (|e2|) ray vectors with respect to the Element axis:

  .. tex:
    e_1 = \begin{bmatrix} y_{1E} \\ V_{1E} \end{bmatrix}
    \qquad
    e_2 = \begin{bmatrix} y_{2E} \\ V_{2E} \end{bmatrix}
  .. image:: img/misalign_eq_02.png

Misalignment vector at the input (|m1|) and output (|m2|):

  .. tex:
    m_1 = \begin{bmatrix} dy_1 \\ dV \end{bmatrix}
    \qquad
    m_2 = \begin{bmatrix} dy_2 \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_03.png

As usual, here we define `V` as a geometrical angle, not a :ref:`reduced slope <reduced_slope>`.

Assuming the misalignment angle is small, `r` and `e` vectors are related as

  .. tex:
    \begin{aligned}
    & r_1 = e_1 + m_1 \\[5pt]
    & r_2 = e_2 + m_2
    \end{aligned}
  .. image:: img/misalign_eq_04.png

A ray vector measured with respect to the element axis is transformed in the usual way by the ABCD matrix `M` of the element:

  .. tex:
    e_2 = M \times e_1
  .. image:: img/misalign_eq_05.png

Then expressing |e2| and |e1| from |r2| and |r1| we get

  .. tex:
    \begin{aligned}
    & r_2 - m_2 = M(r_1 - m_1) \\[5pt]
    & r_2 = M r_1 - M m_1 + m_2
    \end{aligned}
  .. image:: img/misalign_eq_06.png

The output misalignment vector |m2| is connected to |m1| by

  .. tex:
    m_2 = dM \times m_1
  .. image:: img/misalign_eq_07.png

where `dM` is just a ray transformation matrix for :ref:`free space <elem_empty_range>` of length `L`. Then

  .. tex:
    \begin{aligned}
    & r_2 = M r_1 - M m_1 + dM \; m_1 \\[5pt]
    & r_2 = M r_1 + (dM - M) \; m_1 \\[5pt]
    & r_2 = M r_1 + \Delta r
    \end{aligned}
  .. image:: img/misalign_eq_08.png

We can see that the effect of misalignment on a paraxial system is to add some correction vector to the usual matrix transformation. The correction vector is given by:

  .. tex:
    \Delta r
    \equiv
    \begin{bmatrix} E \\ F \end{bmatrix}
    =
    (dM \; – \; M) \; m_1
    =
    \bigg(\,
      \begin{bmatrix} 1 & L \\ 0 & 1 \end{bmatrix} -
      \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \bigg)\,
    \begin{bmatrix} dy_1 \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_09.png

Next we can rename |dy1| with just `dy`, since it is an element parameter, the same as `dV`. Then

  .. tex:
    \begin{bmatrix} E \\ F \end{bmatrix} =
    \begin{bmatrix} 1-A & L-B \\ -C & 1-D \end{bmatrix}
    \begin{bmatrix} dy \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_10.png

These results can be put into a convenient 3x3 matrix form by adding a third unity component to ray vectors:

  .. tex:
    \begin{bmatrix} Y_{2R} \\ V_{2R} \\ 1 \end{bmatrix} =
    \begin{bmatrix} A & B & E \\ C & D & F \\ 0 & 0 & 1 \end{bmatrix}
    \begin{bmatrix} Y_{1R} \\ V_{1R} \\ 1 \end{bmatrix}
  .. image:: img/misalign_eq_11.png

where `A`, `B`, `C`, `D` are components of the element ray matrix, and

  .. tex:
    \begin{aligned}
    & E = (1-A) \; dy + (L-B) \; dV \\[5pt]
    & F = -C \; dy + (1-D) \; dV
    \end{aligned}
  .. image:: img/misalign_eq_12.png

Cascaded Misaligned Elements
----------------------------

When we have a system of several misaligned elements, their extended ray matrices can be multiplied as usual to produce a :ref:`round-trip <round_trip>` matrix of the whole system:

  .. tex:
    \begin{bmatrix} A_0 & B_0 & E_0 \\ C_0 & D_0 & F_0 \\ 0 & 0 & 1 \end{bmatrix} =
    \begin{bmatrix} A_2 & B_2 & E_2 \\ C_2 & D_2 & F_2 \\ 0 & 0 & 1 \end{bmatrix} \times
    \begin{bmatrix} A_1 & B_1 & E_1 \\ C_1 & D_1 & F_1 \\ 0 & 0 & 1 \end{bmatrix}
  .. image:: img/misalign_eq_13.png

When we do the multiplication, it can be seen that the additional matrix components do not affect the basic ray vector transformation:

  .. tex:
    \begin{bmatrix}
    A_2 A_1 + B_2 C_1   &   A_2 B_1 + B_2 D_1   &   A_2 E_1 + B_2 F_1 + E_2 \\
    C_2 A_1 + D_2 C_1   &   C_2 B_1 + D_2 D_1   &   C_2 E_1 + D_2 F_1 + F_2 \\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_eq_14.png

And transformation of `E` and `F` components can be written independently even not involving extended matrices:

  .. tex:
    \begin{bmatrix}E_0 \\ F_0\end{bmatrix} =
    \begin{bmatrix}A_2 & B_2 \\ C_2 & D_2 \end{bmatrix}
    \begin{bmatrix}E_1 \\ F_1\end{bmatrix} +
    \begin{bmatrix}E_2 \\ F_2\end{bmatrix}
  .. image:: img/misalign_eq_15.png

So for an arbitrary number of elements, the full round-trip matrix of the system is calculated as

  .. tex:
    \begin{bmatrix}
    A_0 & B_0 & E_0 \\
    C_0 & D_0 & F_0 \\
    0 & 0 & 1
    \end{bmatrix}
    =
    \begin{bmatrix}
    A_n & B_n & E_n \\
    C_n & D_n & F_n \\
    0 & 0 & 1
    \end{bmatrix}
    \times
    \; ... \;
    \times
    \begin{bmatrix}
    A_2 & B_2 & E_2 \\
    C_2 & D_2 & V_2 \\
    0 & 0 & 1
    \end{bmatrix}
    \times
    \begin{bmatrix}
    A_1 & B_1 & E_1 \\
    C_1 & D_1 & V_1 \\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_eq_16.png

Or it can be written as the conventional 2x2 round-trip matrix plus an additional misalignment vector calculated independently:

  .. tex:
    \begin{aligned}
    & \begin{bmatrix} E_0 \\ F_0 \end{bmatrix} =
    M_n M_{n-1} ... M_3 M_2 \begin{bmatrix} E_1 \\ F_1 \end{bmatrix} +
    M_n M_{n-1} ... M_3 \begin{bmatrix} E_2 \\ F_2 \end{bmatrix} +
    \; ... \;
    M_n \begin{bmatrix} E_{n-1} \\ F_{n-1} \end{bmatrix} +
    \begin{bmatrix} E_n \\ F_n \end{bmatrix}
    \\[10pt]
    & \begin{bmatrix} E_0 \\ F_0 \end{bmatrix} =
    \sum_{r=1}^n \bigg(\, \prod_{i=n}^{r+1} M_i \bigg)\, \begin{bmatrix} E_r \\ F_r \end{bmatrix}
    \end{aligned}
  .. image:: img/misalign_eq_17.png

Effective Optical Axis
----------------------

A system with misaligned elements having the overall matrix `A`, `B`, `C`, `D` and the overall misalignment vector `E`, `F` (here and forward we omit the subscript 0 for shortness) can be converted into an effectively aligned system by translation of the system to the distance |y_eff| from the reference optical axis and rotation it to the angle |V_eff|.

  .. image:: img/misalign_overall_axis.png

Here the expressions for |y_eff| and |V_eff| has been derived by solving a system of linear equations for `E` and `F` expressed via element's matrix components and element's misalignment parameters (see above):

  .. tex:
    \begin{aligned}
    & dy_{eff} = \cfrac{(1-D)E - (L-B)F}{(1-A)(1-D) + (L-B)C} \\[5pt]
    & dV_{eff} = \cfrac{CE + (1-A)F}{(1-A)(1-D) + (L-B)C}
    \end{aligned}
  .. image:: img/misalign_eq_18.png

The vector (|y_eff|, |V_eff|) gives a new effective axis position of the misaligned system with respect to the reference optical axis. Such system looks as if it is perfectly aligned even though its internal elements are misaligned.

Effective Optical Axis of Resonator
-----------------------------------

A resonator can be unwrapped to into an equivalent periodic system, and each period (round-trip) treated as a single element with its own effective axis. Then each round-trip will look like a perfectly aligned system. The problem is this effective axis does not come to itself after one round-trip. So a beam passing through the resonator does not reproduce itself after the round-trip. Hence the resonator is not stable.

  .. image:: img/misalign_resonator_axis.png

So a condition for the effective axis of the resonator should be different from the one for the single-pass system and match the condition of the :ref:`self-conformed beam <self_conformed_beam>` but account for the misalignment of the resonator round-trip:

  .. tex:
    \begin{aligned}
    & \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} +
    \begin{bmatrix} E \\ F \end{bmatrix} =
    \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} \\[10pt]
    & \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} =
    {\bigg(\,
    \begin{bmatrix} 1 & 0 \\ 0 & 1 \end{bmatrix} -
    \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \bigg)\,}^{-1}
    \begin{bmatrix} E \\ F \end{bmatrix}
    \end{aligned}
  .. image:: img/misalign_eq_19.png

Solving the equation we get a position of an "axial ray" - a ray reproducing itself after the round-trip in resonator with misaligned elements. This axial ray forms the effective axis of the resonator.

  .. tex:
    \begin{aligned}
    & y_{eff} = \cfrac{(1-D) \, E + BF}{2-A-D} \\[10pt]
    & V_{eff} = \cfrac{CE + (1-A) \, F}{2-A-D}
    \end{aligned}
  .. image:: img/misalign_eq_20.png

The important note is that, unlike the effective axis of the single-pass system, which is a straight line passing through all elements, the effective axis of the resonator is a series of segments. In general, segments even can be curved, if we consider a non-linear element. At each element of the resonator, we can calculate its own (|y_eff|, |V_eff|) defining where the self-conformed (stable) ray passes through the element.

  .. image:: img/misalign_resonator_axis_1.png

.. seeAlso::

  - :doc:`Misalignments (custom function) <func_misalign>`
