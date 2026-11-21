.. _func_caustic:
.. index:: single: caustic (function)

焦散
=======

:menuSelection:`Functions --> Caustic`

该函数沿指定的元件绘制光束半径和波前曲率半径。

参数
----------

    .. image:: img/func_caustic_params.png

元件
~~~~~~~

该选择器允许你指定要沿其绘制焦散的元件。只能选择具有长度参数的元件。这些元件是空程段、晶体等。长度参数通常称为 :param:`L`。

绘图精度
~~~~~~~~~~~~~

这些字段允许你设置结果图中的点数。点数可以直接在 “Number of points” 字段中输入。或者，也可以在 “With step” 字段中指定点之间的距离（绘图步长）。

对话框中的 “Plottable length” 值显示光束在元件中能走过的最长 :ref:`几何光程 <axial_length>`。这成为图的横向范围。对于某些元件（例如 :doc:`matrix/ElemBrewsterPlate`），可绘制长度大于元件长度，因为光轴与测量长度的方向之间存在一个角度。

附加参数
---------------------

.. _func_caustic_aux_params:

:menuSelection:`View --> Function Params`

在该选项卡中，你可以选择要绘制什么——光束半径还是波前曲率半径。该图会为每种绘制模式记住不同的计量单位。

    .. image:: img/func_caustic_aux_params.png

如果元件内部有一个束腰，光束半径图通常比波前曲率图需要更少的点就能保持准确。这是因为波前曲率半径在近场区变化很快，不能像在远场区那样用少数几个点来近似。

    .. image:: img/func_caustic_r_1.png

    .. image:: img/func_caustic_r_2.png

.. _func_caustic_spec_points:

特殊点
--------------

:menuSelection:`View --> Special Points`

该函数计算绘制范围起点和终点处的光束半径。此外，如果图中存在束腰，也会显示其位置和半径。见黄色高亮的值。基于找到的束腰尺寸，还会计算光束的瑞利距离 (`z`\ :sub:`0`) 和发散角的一半 (`V`\ :sub:`S`)，分别以弧度和度表示。

  .. image:: img/func_caustic_spec_points.png


.. _func_caustic_aux_cmds:

绘图窗口的附加命令
----------------------------------

显示光束形状
~~~~~~~~~~~~~~~

:menuSelection:`View --> Beam Shape`

该函数可以示意性地渲染 :ref:`光标 <cursor>` 位置处的光束横截面，以可视化光束的像散。

光束形状面板可以用鼠标在图上移动。当你把鼠标移到面板附近、其边框显示为细虚线时，点击鼠标还可以调整面板大小。

光束形状图像可以与整个图分开复制；使用其上下文菜单即可。

  .. image:: img/func_caustic_shape.png

.. seeAlso::

    - :doc:`plot_window`
    - :doc:`plot_opers`
    - :doc:`func_caustic_mr`
    - :doc:`func_caustic_mb`
