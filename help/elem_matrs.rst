.. _elem_matrs:
.. index:: single: matrices

Elements Matrices
=================

.. |Mt| replace:: `M`\ :sub:`T`
.. |Ms| replace:: `M`\ :sub:`S`
.. |M_lf| replace:: `M`\ :sup:`LF`
.. |M_rt| replace:: `M`\ :sup:`RT`

|rezonator| utilizes the conventional ABCD-matrix approach (by Kogelnik and Li) to compute the :ref:`propagation <propagation_dir>` of paraxial :doc:`Gaussian beams <gauss>` through an element. Each is element represented in calculations by its ray matrix.

At least, there are two ray matrices for each element - one for tangential and another for sagittal :ref:`working planes <working_planes>`. These are so-called *T-matrix* and *S-matrix*, and they are denoted as |Mt| and |Ms| respectively.

Several elements have more matrices used in some specific calculations. For example, elements that have the length parameter also have two additional matrix sets. The first one describes beam propagation from outside the left edge of the element to some position inside the element. The second one describes propagation from that position to outside the right edge of the element. |rezonator| uses these matrices for computation of beam traveling through the interior of an optical element. They are denoted as |M_lf| and |M_rt| respectively.

.. _interface_elems:

Interface elements have no index of refraction. Instead, they take indexes of adjacent elements to calculate their matrices. So there is some obvious suggestion about the placement of an interface element in the schema - the left and the right adjacent elements of the interface should be either :doc:`matrix/ElemEmptyRange` or :doc:`matrix/ElemMediaRange`. |rezonator| doesn't complain if it is not, but then one can't count on correct results.


.. tocTree::

   matrix/ElemAxiconLens
   matrix/ElemAxiconMirror
   matrix/ElemBrewsterCrystal
   matrix/ElemBrewsterInterface
   matrix/ElemBrewsterPlate
   matrix/ElemMatrix
   matrix/ElemEmptyRange
   matrix/ElemFlatMirror
   matrix/ElemGaussAperture
   matrix/ElemGaussApertureLens
   matrix/ElemGaussDuctMedium
   matrix/ElemGaussDuctSlab
   matrix/ElemGrinLens
   matrix/ElemGrinMedium
   matrix/ElemNormalInterface
   matrix/ElemPlate
   matrix/ElemPoint
   matrix/ElemThinCylinderLensS
   matrix/ElemMediaRange
   matrix/ElemSphericalInterface
   matrix/ElemCurveMirror
   matrix/ElemThinCylinderLensT
   matrix/ElemThermoLens
   matrix/ElemThermoMedium
   matrix/ElemThickLens
   matrix/ElemThinLens
   matrix/ElemTiltedCrystal
   matrix/ElemTiltedInterface
   matrix/ElemTiltedPlate


.. seeAlso::

    - :doc:`../catalog`
    - :doc:`../elem_props`
