.. index:: single: elements
.. index:: single: optical elements

********
Elements
********

Elements are construction blocks of any schema in reZonator.

The element can describe a general optical component such as a :doc:`mirror <matrix/ElemCurveMirror>` or :doc:`crystal <matrix/ElemBrewsterCrystal>`. Additionally, the element can express some physical but non-material entity - a :doc:`distance <matrix/ElemEmptyRange>` between optical components, for example. Finally, the element can also be some helper virtual object - :doc:`point <matrix/ElemPoint>`, for example.

reZonator supports a number of standard elements. They all are contained in the :doc:`catalog` and can be put into a schema from it.

The application utilizes conventional ABCD-matrix approach (by Kogelnik and Li) to compute the propagation of paraxial Gaussian beams through an element.  In calculations each element is represented by its :doc:`ray matrix <elem_matrs>`.

You have not to assign matrices for element manually. Instead, you have to provide values for some typical element parameters - a focal length for a lens, thickness and refractive index for a crystal and so on. And the software will automatically compute element matrices based on these parameters' values.

.. toctree::
   :maxdepth: 1
   :caption: Table of Contents:

   elem_matrs
