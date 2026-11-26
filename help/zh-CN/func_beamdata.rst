各元件处的光束参数
===========================

:menuSelection:`Functions --> Beam Parameters at Elements`

该函数计算光路中每个元件处的光束参数。计算得到的一组值显示在一个表格中。表格的第一列标明元件。

  .. image:: img/func_beamdata.png

列
-------

光束半径 (Wt × Ws)
~~~~~~~~~~~~~~~~~~~~~

元件处的光束半径。显示的计量单位可以通过列标题的上下文菜单或 “Units of measurement” 工具栏按钮更改。

.. TODO: Aperture ratio (At × As)
.. TODO: ~~~~~~~~~~~~~~~~~~~~~~~~

.. TODO: The ratio of the element's aperture to the beam radius at the element: `A = (D/2) / w` where `D` is the element's aperture diameter.

波前曲率半径 (Rt × Rs)
~~~~~~~~~~~~~~~~~~~~~~~

元件处的波前曲率半径。显示的计量单位可以通过列标题的上下文菜单或 “Units of measurement” 工具栏按钮更改。

发散半角 (Vt × Vs)
~~~~~~~~~~~~~~~~~~~~~~~~~

远场区发散角的一半。显示的计量单位可以通过列标题的上下文菜单或 “Units of measurement” 工具栏按钮更改。

.. note::
  表格中可能有几行对应同一个元件。这些行以不同的图标标记。详情见 :doc:`table_symbols`。

.. seeAlso::

  - :doc:`table_window`
  - :doc:`table_opers`
  - :doc:`func_caustic`
