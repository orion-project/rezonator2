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

  .. image:: img/calc_grin_f1.png

Then similarly, we can express the ray (|r3|, |v3|) from the ray (|r2|, |v2|) and matrix of the :doc:`free space <matrix/ElemEmptyRange>`:

  .. image:: img/calc_grin_f2.png

Then combining these two expressions and solving against `F`, we can get the final formula:

  .. image:: img/calc_grin_f0.png


Gradient Constant Assessment
----------------------------

There is no such simple formula for gradient index |n2|, so it's computed via numerical solution of the above formula agains |n2| having known `F`, `L`, and |n0|.
