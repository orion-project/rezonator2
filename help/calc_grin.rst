GRIN Lens Assessment
====================

.. |n0| replace:: `n`\ :sub:`0`
.. |n2| replace:: `n`\ :sub:`2`
.. |r1| replace:: `r`\ :sub:`1`
.. |v1| replace:: `V`\ :sub:`1`
.. |r2| replace:: `r`\ :sub:`2`
.. |v2| replace:: `V`\ :sub:`2`
.. |r3| replace:: `r`\ :sub:`3`
.. |v3| replace:: `V`\ :sub:`3`

:menuselection:`Tools --> GRIN Lens Assessment`

The tool *GRIN Lens Assessment* allows for calculation of GRIN-lens focal range `F` from known length `L`, axial IOR |n0|, and gradient constant |n2|.

Or it's possible to calculate gradient constant |n2| from known focal range `F`, length `L`, and axial IOR |n0|. In any case, `L` and |n0| are always input parameters. 

  .. image:: img/calc_grin.png

Buttons on the toolbar define which of the values |n2| or `F` calculated when we change one of `L` or |n0|.
 
Because |n2| and `F` are free parameters here, then when one of them changes, the other is calculated independently on which button pressed on the toolbar.

  
Focus Range Assessment
----------------------

A geometric approach used to estimate the focus range of the GRIN-lens. It's done according to the book "Quantum Electronics" by A. Yariv, see chapter 6.4 "Rays in lenslike media" (there is no exact algorithm there but the final formula is given).

  .. image:: img/calc_grin_draw.png
  
We have a :doc:`matrix/ElemGrinLens` and a parallel input ray (|r1|, |v1|) entering it. Then an output ray (|r2|, |v2|) can be calculated with :ref:`ray vector transformation <ray_vector>` formula: 

  .. tex:
    \begin{aligned}
    & \begin{bmatrix}r_2 \\ V_2 \end{bmatrix} =
      \begin{bmatrix}
        \cos(\gamma L) & \cfrac {\sin(\gamma L)} {n_0 \gamma} \\
        -n_0 \gamma \: \sin(\gamma L) & \cos(\gamma L) 
      \end{bmatrix}
      \begin{bmatrix}r_1 \\ V_1 \end{bmatrix} \\ \\
    & V_1 = 0 \\ \\
    & r_2 = r_1 \cos(\gamma L) \\ \\
    & V_2 = -r_1 n_0 \gamma \sin(\gamma L)
    \end{aligned}
  .. image:: img/calc_grin_f1.png

Then similarly, we can express the ray (|r3|, |v3|) from the ray (|r2|, |v2|) and matrix of the :doc:`free space <matrix/ElemEmptyRange>`:

  .. tex:
    \begin{aligned}
    & \begin{bmatrix}r_3 \\ V_3 \end{bmatrix} =
      \begin{bmatrix}
        1 & F \\
        0 & 1 
      \end{bmatrix}
    \begin{bmatrix}r_2 \\ V_2 \end{bmatrix} \\ \\
    & r_3 = r_2 + F V_2 = 0 \\ \\
    & V_3 = V_2
    \end{aligned}
  .. image:: img/calc_grin_f2.png

Then combining these two expressions and solving against `F`, we can get the final formula:

  .. tex: F = \frac 1 {n_0 \gamma} \frac {\cos(\gamma L)}{\sin(\gamma L)} = \frac 1 {n_0 \gamma \tan(\gamma L)}
  .. image:: img/calc_grin_f0.png

Negative lens
^^^^^^^^^^^^^

  .. image:: img/calc_grin_draw_neg.png

In a similar way we can get the focal length of negative thermal lens:

  .. tex:
    \begin{aligned}
    & \begin{bmatrix}r_2 \\ V_2 \end{bmatrix} =
      \begin{bmatrix}
        \cosh(\gamma L)                &   \cfrac {\sinh(\gamma L)} {n_0 \gamma} \\
        n_0 \gamma \: \sinh(\gamma L)  &   \cosh(\gamma L) 
      \end{bmatrix}
      \begin{bmatrix}r_1 \\ V_1 \end{bmatrix} \\ \\
    & V_1 = 0 \\ \\
    & r_2 = r_1 \cosh(\gamma L) \\ \\
    & V_2 = r_1 n_0 \gamma \sinh(\gamma L)
    \end{aligned}
  .. image:: img/calc_grin_f1_neg.png

It can be seen from the figure that vector radius |r3| is doubled after passing the focal length:

  .. tex:
    \begin{aligned}
    & \begin{bmatrix}r_3 \\ V_3 \end{bmatrix} =
      \begin{bmatrix}
        1 & F \\
        0 & 1 
      \end{bmatrix}
    \begin{bmatrix}r_2 \\ V_2 \end{bmatrix} \\ \\
    & r_3 = r_2 + F V_2 = 2 r_2 \\ \\
    & V_3 = V_2
    \end{aligned}
  .. image:: img/calc_grin_f2_neg.png

So combining these two expressions and solving against `F`, we get a formula which is quite similar to that of positive lens:

  .. tex: F = - \frac 1 {n_0 \gamma} \frac {\cosh(\gamma L)}{\sinh(\gamma L)} = - \frac 1 {n_0 \gamma \tanh(\gamma L)}
  .. image:: img/calc_grin_f0_neg.png

Gradient Constant Assessment
----------------------------

There is no such simple formula for gradient index |n2|, so it's computed via numerical solution of the above formula against |n2| having known `F`, `L`, and |n0|.

