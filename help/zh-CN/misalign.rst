.. _misalign:

失调的计算
============================

.. |r1| replace:: `r`\ :sub:`1`
.. |r2| replace:: `r`\ :sub:`2`
.. |e1| replace:: `e`\ :sub:`1`
.. |e2| replace:: `e`\ :sub:`2`
.. |m1| replace:: `m`\ :sub:`1`
.. |m2| replace:: `m`\ :sub:`2`
.. |dy1| replace:: `dy`\ :sub:`1`
.. |y_eff| replace:: `dy`\ :sub:`eff`
.. |V_eff| replace:: `dV`\ :sub:`eff`

假设我们有一个元件，它的轴相对于光学系统轴（参考轴）偏移了 |dy1|，并旋转了 `dV`。我们关心的是，这种偏移和旋转（失调）如何影响穿过这样一个元件或一组元件的光线。我们以参考光轴为基准来测量光线。

  .. image:: img/misalign_calc.png

ABCDEF 光线矩阵的推导
---------------------------------

下面引入以下符号。

相对于参考轴的输入（|r1|）和输出（|r2|）光线矢量：

  .. tex:
    r_1 = \begin{bmatrix} y_{1R} \\ V_{1R} \end{bmatrix}
    \qquad
    r_2 = \begin{bmatrix} y_{2R} \\ V_{2R} \end{bmatrix}
  .. image:: img/misalign_eq_01.png

相对于元件轴的输入（|e1|）和输出（|e2|）光线矢量：

  .. tex:
    e_1 = \begin{bmatrix} y_{1E} \\ V_{1E} \end{bmatrix}
    \qquad
    e_2 = \begin{bmatrix} y_{2E} \\ V_{2E} \end{bmatrix}
  .. image:: img/misalign_eq_02.png

输入处（|m1|）和输出处（|m2|）的失调矢量：

  .. tex:
    m_1 = \begin{bmatrix} dy_1 \\ dV \end{bmatrix}
    \qquad
    m_2 = \begin{bmatrix} dy_2 \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_03.png

与通常一样，这里我们把 `V` 定义为几何角度，而不是 :ref:`折合斜率 <reduced_slope>`。

假设失调角很小，则 `r` 矢量和 `e` 矢量之间的关系为

  .. tex:
    \begin{aligned}
    & r_1 = e_1 + m_1 \\[5pt]
    & r_2 = e_2 + m_2
    \end{aligned}
  .. image:: img/misalign_eq_04.png

以元件轴为基准测量的光线矢量，按通常方式由元件的 ABCD 矩阵 `M` 进行变换：

  .. tex:
    e_2 = M \times e_1
  .. image:: img/misalign_eq_05.png

然后由 |r2| 和 |r1| 表示 |e2| 和 |e1|，我们得到

  .. tex:
    \begin{aligned}
    & r_2 - m_2 = M(r_1 - m_1) \\[5pt]
    & r_2 = M r_1 - M m_1 + m_2
    \end{aligned}
  .. image:: img/misalign_eq_06.png

输出失调矢量 |m2| 与 |m1| 的关系为

  .. tex:
    m_2 = dM \times m_1
  .. image:: img/misalign_eq_07.png

其中 `dM` 就是长度为 `L` 的 :ref:`自由空间 <elem_empty_range>` 的光线变换矩阵。然后

  .. tex:
    \begin{aligned}
    & r_2 = M r_1 - M m_1 + dM \; m_1 \\[5pt]
    & r_2 = M r_1 + (dM - M) \; m_1 \\[5pt]
    & r_2 = M r_1 + \Delta r
    \end{aligned}
  .. image:: img/misalign_eq_08.png

我们可以看到，失调对近轴系统的影响是给通常的矩阵变换加上一个修正矢量。该修正矢量由下式给出：

  .. tex:
    \Delta r
    \equiv
    \begin{bmatrix} E \\ F \end{bmatrix}
    =
    (dM \; – \; M) \; m_1
    =
    \bigg(\,
      \begin{bmatrix} 1 & L \\ 0 & 1 \end{bmatrix} -
      \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \bigg)\,
    \begin{bmatrix} dy_1 \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_09.png

接下来我们可以把 |dy1| 改记为 `dy`，因为它和 `dV` 一样，是一个元件参数。然后

  .. tex:
    \begin{bmatrix} E \\ F \end{bmatrix} =
    \begin{bmatrix} 1-A & L-B \\ -C & 1-D \end{bmatrix}
    \begin{bmatrix} dy \\ dV \end{bmatrix}
  .. image:: img/misalign_eq_10.png

通过给光线矢量加上第三个单位分量，可以把这些结果写成方便的 3x3 矩阵形式：

  .. tex:
    \begin{bmatrix} Y_{2R} \\ V_{2R} \\ 1 \end{bmatrix} =
    \begin{bmatrix} A & B & E \\ C & D & F \\ 0 & 0 & 1 \end{bmatrix}
    \begin{bmatrix} Y_{1R} \\ V_{1R} \\ 1 \end{bmatrix}
  .. image:: img/misalign_eq_11.png

其中 `A`、`B`、`C`、`D` 是元件光线矩阵的分量，并且

  .. tex:
    \begin{aligned}
    & E = (1-A) \; dy + (L-B) \; dV \\[5pt]
    & F = -C \; dy + (1-D) \; dV
    \end{aligned}
  .. image:: img/misalign_eq_12.png

级联的失调元件
----------------------------

当我们有一个由几个失调元件组成的系统时，它们的扩展光线矩阵可以像通常一样相乘，得到整个系统的 :ref:`往返 <round_trip>` 矩阵：

  .. tex:
    \begin{bmatrix} A_0 & B_0 & E_0 \\ C_0 & D_0 & F_0 \\ 0 & 0 & 1 \end{bmatrix} =
    \begin{bmatrix} A_2 & B_2 & E_2 \\ C_2 & D_2 & F_2 \\ 0 & 0 & 1 \end{bmatrix} \times
    \begin{bmatrix} A_1 & B_1 & E_1 \\ C_1 & D_1 & F_1 \\ 0 & 0 & 1 \end{bmatrix}
  .. image:: img/misalign_eq_13.png

进行乘法时可以看到，附加的矩阵分量不影响基本的光线矢量变换：

  .. tex:
    \begin{bmatrix}
    A_2 A_1 + B_2 C_1   &   A_2 B_1 + B_2 D_1   &   A_2 E_1 + B_2 F_1 + E_2 \\
    C_2 A_1 + D_2 C_1   &   C_2 B_1 + D_2 D_1   &   C_2 E_1 + D_2 F_1 + F_2 \\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_eq_14.png

而且 `E` 和 `F` 分量的变换甚至可以不涉及扩展矩阵而独立写出：

  .. tex:
    \begin{bmatrix}E_0 \\ F_0\end{bmatrix} =
    \begin{bmatrix}A_2 & B_2 \\ C_2 & D_2 \end{bmatrix}
    \begin{bmatrix}E_1 \\ F_1\end{bmatrix} +
    \begin{bmatrix}E_2 \\ F_2\end{bmatrix}
  .. image:: img/misalign_eq_15.png

所以对于任意数量的元件，系统的完整往返矩阵计算为

  .. tex:
    \begin{bmatrix}
    A_0 & B_0 & E_0 \\
    C_0 & D_0 & F_0 \\
    0 & 0 & 1
    \end{bmatrix}
    =
    \begin{bmatrix}
    A_n & B_n & E_n \\
    C_n & D_n & F_n \\
    0 & 0 & 1
    \end{bmatrix}
    \times
    \; ... \;
    \times
    \begin{bmatrix}
    A_2 & B_2 & E_2 \\
    C_2 & D_2 & V_2 \\
    0 & 0 & 1
    \end{bmatrix}
    \times
    \begin{bmatrix}
    A_1 & B_1 & E_1 \\
    C_1 & D_1 & V_1 \\
    0 & 0 & 1
    \end{bmatrix}
  .. image:: img/misalign_eq_16.png

或者它可以写成常规的 2x2 往返矩阵，再加上一个独立计算的附加失调矢量：

  .. tex:
    \begin{aligned}
    & \begin{bmatrix} E_0 \\ F_0 \end{bmatrix} =
    M_n M_{n-1} ... M_3 M_2 \begin{bmatrix} E_1 \\ F_1 \end{bmatrix} +
    M_n M_{n-1} ... M_3 \begin{bmatrix} E_2 \\ F_2 \end{bmatrix} +
    \; ... \;
    M_n \begin{bmatrix} E_{n-1} \\ F_{n-1} \end{bmatrix} +
    \begin{bmatrix} E_n \\ F_n \end{bmatrix}
    \\[10pt]
    & \begin{bmatrix} E_0 \\ F_0 \end{bmatrix} =
    \sum_{r=1}^n \bigg(\, \prod_{i=n}^{r+1} M_i \bigg)\, \begin{bmatrix} E_r \\ F_r \end{bmatrix}
    \end{aligned}
  .. image:: img/misalign_eq_17.png

有效光轴
----------------------

一个具有失调元件的系统，其总矩阵为 `A、B、C、D`，总失调矢量为 `E、F`，从这里开始为简洁起见省略下标 0，可以通过把系统平移到距参考光轴 |y_eff| 的距离处、并旋转到角度 |V_eff|，转换为一个有效对准的系统。

  .. image:: img/misalign_overall_axis.png

这里 |y_eff| 和 |V_eff| 的表达式，是通过求解一个线性方程组得到的，其中 `E` 和 `F` 由元件的矩阵分量和元件的失调参数表示（见上文）：

  .. tex:
    \begin{aligned}
    & dy_{eff} = \cfrac{(1-D)E - (L-B)F}{(1-A)(1-D) + (L-B)C} \\[5pt]
    & dV_{eff} = \cfrac{CE + (1-A)F}{(1-A)(1-D) + (L-B)C}
    \end{aligned}
  .. image:: img/misalign_eq_18.png

矢量（|y_eff|，|V_eff|）给出了失调系统相对于参考光轴的新的有效轴位置。这样的系统看起来仿佛完全对准，尽管其内部元件是失调的。

谐振腔的有效光轴
-----------------------------------

谐振腔可以展开为一个等效的周期系统，每个周期（往返）被视为一个具有自己有效轴的单个元件。然后每次往返看起来都像一个完全对准的系统。问题是，这条有效轴在一次往返后并不回到自身。所以穿过谐振腔的光束在往返后不能再现自身。因此谐振腔是不稳定的。

  .. image:: img/misalign_resonator_axis.png

所以谐振腔有效轴的条件应当不同于单程系统的条件，而应符合 :ref:`自再现光束 <self_conformed_beam>` 的条件，但要考虑谐振腔往返的失调：

  .. tex:
    \begin{aligned}
    & \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} +
    \begin{bmatrix} E \\ F \end{bmatrix} =
    \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} \\[10pt]
    & \begin{bmatrix} y_{eff} \\ V_{eff} \end{bmatrix} =
    {\bigg(\,
    \begin{bmatrix} 1 & 0 \\ 0 & 1 \end{bmatrix} -
    \begin{bmatrix} A & B \\ C & D \end{bmatrix}
    \bigg)\,}^{-1}
    \begin{bmatrix} E \\ F \end{bmatrix}
    \end{aligned}
  .. image:: img/misalign_eq_19.png

求解该方程，我们得到一条 “轴光线” 的位置——这是一条在具有失调元件的谐振腔中往返后能再现自身的光线。这条轴光线构成谐振腔的有效轴。

  .. tex:
    \begin{aligned}
    & y_{eff} = \cfrac{(1-D) \, E + BF}{2-A-D} \\[10pt]
    & V_{eff} = \cfrac{CE + (1-A) \, F}{2-A-D}
    \end{aligned}
  .. image:: img/misalign_eq_20.png

需要注意的是，与单程系统的有效轴不同——那是一条穿过所有元件的直线——谐振腔的有效轴是一系列线段。一般来说，如果考虑非线性元件，线段甚至可以是弯曲的。在谐振腔的每个元件处，我们都可以计算它自己的（|y_eff|，|V_eff|），它定义了自再现（稳定）光线穿过该元件的位置。

  .. image:: img/misalign_resonator_axis_1.png

.. seeAlso::

  - :doc:`失调（自定义函数） <func_misalign>`
