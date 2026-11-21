.. index:: single: lensmaker

透镜设计器
====================

.. |R1| replace:: `R`\ :sub:`1`
.. |R2| replace:: `R`\ :sub:`2`
.. |FF| replace:: `F`\ :sub:`F`
.. |FR| replace:: `F`\ :sub:`R`

:menuselection:`Tools --> Lensmaker`

Lensmaker 工具是一个简单的透镜设计器，用于计算透镜的焦距并绘制其形状。它使用标准的 `透镜制造者方程 <https://en.wikipedia.org/wiki/Lens>`_ 来计算空气中单片透镜的焦距。

    .. image:: img/calc_lens_draw.png

透镜参数：
----------------

`D` —— 透镜直径。它不影响任何计算，仅用于绘图。

|R1| —— 左表面的曲率半径。负值表示表面向右凸出，正值表示表面向左凸出。平面使用 Inf。

|R2| —— 右表面的曲率半径。负值表示表面向右凸出，正值表示表面向左凸出。平面使用 Inf。

`n` —— 透镜材料的折射率。该工具只计算空气中的透镜。

`T` —— 透镜沿光轴的厚度。它是 `V` 点与 `V'` 点之间的距离。

.. note::
  这里使用光学表面的 `标准符号约定 <https://en.wikipedia.org/wiki/Radius_of_curvature_(optics)>`_。

计算结果：
-------------------

`F` —— 有效焦距是焦点与相应主点之间的距离，即 `F - H` 和 `F' - H'`。一般情况下有前、后两个有效焦距，但当透镜前后的介质相同时，它们相等。

  .. tex:
    P = \cfrac{1}{F} = (n - 1)\bigg[\, 
        \cfrac{1}{R_1} - 
        \cfrac{1}{R_2} + 
        \cfrac{(n -1)T}{n R_1 R_2}
    \bigg]\,
  .. image:: img/calc_lens_f.png

`P` —— 以屈光度为单位的光焦度，见上式。

|FF| —— 前焦距是前焦点 `F` 与左表面顶点 `V` 之间的距离。

  .. tex:
    F_F = -F \bigg(\, 1 + \cfrac{n - 1}{n R_2} T \bigg)\,
  .. image:: img/calc_lens_f1.png

|FR| —— 后焦距是后焦点 `F'` 与右表面顶点 `V'` 之间的距离。

  .. tex:
    F_R = F \bigg(\, 1 - \cfrac{n - 1}{n R_1} T \bigg)\,
  .. image:: img/calc_lens_f2.png
