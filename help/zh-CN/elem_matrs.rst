.. _elem_matrs:
.. index:: single: matrices

元件矩阵
=================

.. |Mt| replace:: `M`\ :sub:`T`
.. |Ms| replace:: `M`\ :sub:`S`
.. |M_lf| replace:: `M`\ :sup:`LF`
.. |M_rt| replace:: `M`\ :sup:`RT`

|rezonator| 采用传统的 ABCD 矩阵方法（由 Kogelnik 和 Li 提出）来计算近轴 :doc:`高斯光束 <gauss>` 通过元件的 :ref:`传播 <propagation_dir>`。每个元件在计算中都由其光线矩阵表示。

至少，每个元件都有两个光线矩阵——一个用于切向平面，另一个用于弧矢 :ref:`工作平面 <working_planes>`。它们就是所谓的 *T 矩阵*和 *S 矩阵*，分别记作 |Mt| 和 |Ms|。

有几个元件还有更多用于某些特定计算的矩阵。例如，具有长度参数的元件还有另外两组矩阵。第一组描述光束从元件左边缘外侧传播到元件内部某个位置的过程。第二组描述从该位置传播到元件右边缘外侧的过程。|rezonator| 使用这些矩阵来计算光束在光学元件内部的传播。它们分别记作 |M_lf| 和 |M_rt|。

.. _interface_elems:

界面元件没有折射率。相反，它们采用相邻元件的折射率来计算自己的矩阵。因此，对于界面元件在光路中的放置位置，有一个显而易见的建议——界面的左、右相邻元件应该是 :doc:`matrix/ElemEmptyRange` 或 :doc:`matrix/ElemMediaRange`。如果不是这样，|rezonator| 也不会报错，但那时就不能指望得到正确的结果了。


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
   matrix/ElemFormula


.. seeAlso::

    - :doc:`../catalog`
    - :doc:`../elem_props`
