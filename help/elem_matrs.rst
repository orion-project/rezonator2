.. index:: single: matrices

Elements Matrices
================

.. |Mt| replace:: *M*\ :sub:`T`
.. |Ms| replace:: *M*\ :sub:`S`
.. |M_lf| replace:: *M*\ :sup:`LF`
.. |M_rt| replace:: *M*\ :sup:`RT`

reZonator utilizes conventional ABCD-matrix approach (by Kogelnik and Li) to compute the propagation of paraxial Gaussian beams through an element. Each element is represented in calculations by its ray matrix.

In most cases, there are two ray matrices for each element - matrix for tangential and sagittal planes. These are so-called *T-matrix* and *S-matrix* and they denoted as |Mt| and |Ms| respectively.

For some elements, there are more matrices which are used in some specific calculations. For example, elements that have the length parameter also have two additional matrix set. The first one describes beam propagation from outside of the left edge of the element to some position inside the element. The second one describes propagation from that position to outside of the right edge of the element. These matrices are used for computation of beam traveling through the interior of an optical element. They are denoted as |M_lf| and |M_rt| respectively.


