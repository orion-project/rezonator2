.. index:: single: elements
.. index:: single: optical elements

********
Elements
********

Elements are construction blocks of any schema in |rezonator|.

The element can describe a general optical component such as a :doc:`mirror <matrix/ElemCurveMirror>` or :doc:`crystal <matrix/ElemBrewsterCrystal>`. Additionally, it can express some physical but non-material entity - a :doc:`distance <matrix/ElemEmptyRange>` between optical components, for example. Finally, it can also serve as some virtual helper object - :doc:`point <matrix/ElemPoint>`, for example.

|rezonator| supports a set of standard elements organized in a :doc:`catalog`.

The application utilizes conventional ABCD-matrix approach (by Kogelnik and Li) to compute the propagation of paraxial Gaussian beams through an element.  In calculations, each element represented by its :doc:`ray matrix <elem_matrs>`.

You don't have to assign matrices for elements manually. Instead, you can provide values for some typical element parameters - a focal length of a lens, thickness and refractive index for a crystal, and others. The software automatically computes element matrices based on these parameter values.

.. toctree::
   :maxdepth: 1
   :caption: Table of Contents:

   elem_matrs
