.. index:: single: thermo medium

Thermal medium
==============

.. |n0| replace:: `n`\ :sub:`0`
.. |n2| replace:: `n`\ :sub:`2`

The thermal medium has a quadratic transverse variation of refractive index, with either a maximum or minimum on the axis that produces a lensing effect.

This element is entirely similar to :doc:`ElemGrinMedium`, however instead of setting gradient constant |n2|, you can explicitly assign focal range `F`, and gradient constant is calculated using the :doc:`../calc_grin` tool.

The element is cylindrically symmetrical and doesn't imply differences between tangential and sagittal planes.

    .. image:: ../../img/elem/ElemThermoMedium.svg


.. seealso::

    :doc:`./ElemThermoLens`,
    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Gradient-index optics in Wikipedia <https://en.wikipedia.org/wiki/Gradient-index_optics>`_
