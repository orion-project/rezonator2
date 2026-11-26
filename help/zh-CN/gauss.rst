.. index:: single: gaussian beam

高斯光束
=============

.. |z0| replace:: `z`\ :sub:`0`

高斯光束是具有衍射发散的相干辐射光束的模型。光束的能量集中在其轴线附近（近轴光束），并在横向方向上迅速衰减。光束强度的径向衰减恰当地由高斯函数刻画。这样的光束是单条光线最精确的衍射近似。

径向强度变化
--------------------------

光束强度随径向距离的变化：

    .. image:: img/gauss_intensity.png
    
约 86.5% 的光束总能量集中在近轴区域：

    .. image:: img/gauss_energy.png
  
  
.. index:: single: width at half-maximum
.. index:: single: full width at half-maximum
.. index:: single: FWHM
    
强度
---------

高斯光束的强度在约为光斑尺寸 0.6 倍的距离处下降为一半：

    .. image:: img/gauss_fwhm.png
    
|para|

    .. image:: img/gauss_profile.png
    
|para|

    .. image:: img/gauss_profiles.png
    
    
    
.. _gauss-radius:
.. index:: single: radius of gaussian beam
.. index:: single: spot size of gaussian beam

半径（光斑尺寸）
------------------

光束半径沿光束传播轴的变化：

    .. image:: img/gauss_size_z.png
    
 
.. index:: single: wavefront of gaussian beam
.. index:: single: curvature of wavefront of gaussian beam
.. index:: single: radius of wavefront curvature of gaussian beam

波前曲率半径
-----------------------------

波前曲率半径沿光束传播轴的变化：

    .. image:: img/gauss_front_z.png
    
   
.. _gauss_confocal:
.. index:: single: rayleigh distance
.. index:: single: far-field zone
.. index:: single: near-field zone

瑞利距离
-----------------

瑞利距离是这样一个距离：在该处波前曲率半径最小（因此曲率最大），且光斑面积是束腰面积的两倍。距离 `z` > |z0| 称为**远场区**。距离 `z` < |z0| 称为**近场区**。

    .. image:: img/gauss_z0.png

|para|

    .. image:: img/gauss_curvature.png

 
.. index:: single: divergence of gaussian beam
.. index:: single: angle of gaussian beam
 
角发散
--------------
 
高斯光束的角发散由远场区的渐近发散角给出。正负号意味着该公式只给出光束全角的一半。

    .. image:: img/gauss_vs.png
    
|para|

    .. image:: img/gauss_caustic.png
    
|para|

    .. image:: img/gauss_caustics.png
    

    
.. _gauss_complex_param:
.. index:: single: complex radius of curvature
    
复曲率半径
---------------------------

复曲率半径 `q(z)` 通过以下定义与光束半径 `w(z)` 和曲率半径 `R(z)` 相关联：

    .. image:: img/gauss_q.png
    
光束半径和曲率半径可以由复曲率半径表示出来：

    .. image:: img/gauss_q_r_w.png
    

.. seealso::

    :doc:`matrix_optics`, :doc:`hyper_gauss`

