.. index:: single: matrix optics

谐振腔中光束参数的计算
============================================

本说明依据 A.E. Siegman 的著作《Lasers》，见第 21 章 “Generalized paraxial resonator theory”。

为了进行分析，我们需要选择一个参考平面，相对于它来计算 :doc:`往返矩阵<round_trip_matrix>`。一般而言，被选作某个函数参数的元件就是参考平面。

.. _self_conformed_beam:

有了往返矩阵，我们可以写出 :ref:`复参数<gauss_complex_param>` 的变换表达式：

    .. image:: img/gauss_q_abcd.png

由于我们考虑的是稳定谐振腔，输出参数与输入参数相同，因此光束是自洽的，它在谐振腔中完成一次完整往返后能再现自身：

    .. image:: img/abcd_q_in_out.png

|para|

    .. image:: img/abcd_q.png

然后我们可以求出复参数，它是二次方程的一个解：

    .. image:: img/abcd_q_quadratic.png


但为了方便，最好将该方程改写为倒数复参数的形式：

    .. image:: img/abcd_q_quadratic_1.png


利用关系式

    .. image:: img/abcd_ad_bc.png

自洽复曲率半径
------------------------------------------

该方程的解给出谐振腔中自洽厄米-高斯模式的复参数值：

    .. image:: img/abcd_solution.png

或

    .. image:: img/abcd_solution_1.png


其中一个解给出不真实的情况，即光束能量随径向距离增大。

知道复参数通过光束特性的表达式

    .. image:: img/gauss_q.png

我们可以计算谐振腔中自洽光束的半径和曲率半径：

    .. image:: img/gauss_q_r_w.png

.. ##################################################################

.. _abcd_beam:

用矩阵分量表示的光束性质
---------------------------------------------

.. _abcd_beam_roc:

自洽光束的曲率半径：

    .. image:: img/abcd_curvature.png

.. _abcd_beam_size:

自洽光束半径：

    .. image:: img/abcd_radius_1.png

|para|

    .. image:: img/abcd_radius_2.png

|para|

    .. image:: img/abcd_radius_3.png

.. _abcd_beam_angle:

远场区发散半角：

    .. tex:
        V_S^2 = \cfrac{\lambda}{\pi} \cfrac{2 C}{\sqrt{4 - (A + D)^2}}

    .. image:: img/abcd_angle.png

这些公式仅供参考。当矩阵分量是复数时它们并不适用，因此本应用实际上不使用它们，而是直接由自洽的 `q` 参数计算光束性质。

.. seeAlso::

    - :doc:`gauss`
