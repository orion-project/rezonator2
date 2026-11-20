.. index:: single: RoundTrip (Python)

类：RoundTrip
================

用于光束参数和稳定性的 :ref:`Round-trip <round_trip>` 计算器。

.. note::

  RoundTrip 对象不能直接创建；使用 :ref:`schema.round_trip() <py_method_schema_round_trip>` 函数来获取它们。

属性
----------

``ref`` (:doc:`Element <py_element>`, read-only)
    往返的参考元件

.. _py_method_rt_plane:

``plane`` (int, read-only)
    :ref:`Work plane <work_plane>`（:ref:`Z.PLANE_T <py_const_plane_t>` 或 :ref:`Z.PLANE_S <py_const_plane_t>`）

``ior`` (float, read-only)
    当前的折射率。折射率会根据参考元件是什么而自动赋值。例如，如果元件是一个 :ref:`interface <ElemNormalInterface>`，则折射率将取自紧随该界面之后的 :ref:`medium <ElemMediumRange>` 元件。

``matrix_count`` (int, read-only)
    往返中矩阵的数量

``stabil_nor`` (float, read-only)
    :ref:`Stability parameter <stability_param>`（普通模式）：`P = (A + D)/2`

``stabil_sqr`` (float, read-only)
    :ref:`Stability parameter <stability_param>`（平方模式）：`P = 1 - ((A + D)/2)²`

方法
-------

.. ##################################################################

.. index:: single: matrix (Python)
.. _py_method_rt_matrix:

``matrix(index=None)``
~~~~~~~~~~~~~~~~~~~~~~

计算并返回参考点处的往返矩阵。

参数：

- ``index`` (int, optional) - 矩阵索引，或 None 表示总的往返矩阵

返回值：

- :doc:`Matrix <py_matrix>` - 请求的矩阵

引发：

- ``IndexError`` - 如果索引无效

.. ##################################################################

.. index:: single: beam_radius (Python)
.. _py_method_rt_beam_radius:

``beam_radius()``
~~~~~~~~~~~~~~~~~

计算参考点处的往返矩阵，并返回由矩阵分量计算出的 :ref:`beam radius <abcd_beam_size>`。

返回值：

- float - 以米为单位的光束半径

.. ##################################################################

.. index:: single: front_radius (Python)
.. _py_method_rt_front_radius:

``front_radius()``
~~~~~~~~~~~~~~~~~~

计算参考点处的往返矩阵，并返回由矩阵分量计算出的 :ref:`wavefront radius of curvature <abcd_beam_roc>`。

返回值：

- float - 以米为单位的波前半径

.. ##################################################################

.. index:: single: half_angle (Python)
.. _py_method_rt_half_angle:

``half_angle()``
~~~~~~~~~~~~~~~~

计算参考点处的往返矩阵，并返回由矩阵分量计算出的 :ref:`half of the divergence angle in the far field <abcd_beam_angle>`。


返回值：

- float - 以弧度为单位的半角

.. ##################################################################

.. index:: single: beam (Python)
.. _py_method_rt_beam:

``beam()``
~~~~~~~~~~

计算参考点处的往返矩阵，并返回由矩阵分量计算出的 :ref:`all beam properties <abcd_beam>`。

返回值：

- tuple(float, float, float) - (beam_radius, front_radius, half_angle)，单位分别为 m、m、rad

.. ##################################################################

.. note::

  在每次计算光束属性之前（例如 ``beam_radius()``），都会自动进行 :ref:`Round-trip matrix <round_trip>` 的计算。这保证了计算会考虑元件的实际参数。对于同一属性的多次查询，应将结果存入局部变量，而不是重复计算。

.. ##################################################################

.. index:: single: elem (Python)
.. _py_method_rt_elem:

``elem(index)``
~~~~~~~~~~~~~~~

获取拥有指定矩阵的元件。

参数：

- ``index`` (int) - 矩阵索引

返回值：

- :doc:`Element <py_element>` - 拥有此矩阵的元件

引发：

- ``IndexError`` - 如果索引无效

.. ##################################################################

示例
-------

.. code-block:: python

    import rezonator as Z
    import schema

    def calculate():
        ref_elem = schema.elem(4)

        # Set offset inside element
        if ref_elem.length is not None:
            ref_elem.offset = ref_elem.length / 2.0

        # Create round trip calculators for both planes
        rt_t = schema.round_trip(ref=ref_elem, inside=True, plane=Z.PLANE_T)
        rt_s = schema.round_trip(ref=ref_elem, inside=True, plane=Z.PLANE_S)

        # Get matrices
        Z.print('Tangential matrix:', rt_t.matrix())
        Z.print('Sagittal matrix:', rt_s.matrix())

        # Get stability parameters
        Z.print('Stability (T, normal):', rt_t.stabil_nor)
        Z.print('Stability (S, normal):', rt_s.stabil_nor)
        Z.print('Stability (T, squared):', rt_t.stabil_sqr)
        Z.print('Stability (S, squared):', rt_s.stabil_sqr)

        # Get beam parameters
        w_t = rt_t.beam_radius()
        w_s = rt_s.beam_radius()
        Z.print(f'Beam size: {w_t*1e3:.3f} × {w_s*1e3:.3f} mm')

.. seeAlso::

  - :doc:`py_api`
  - :doc:`py_element`
  - :doc:`py_matrix`
  - :ref:`Example - Round Trip and Beam Properties <py_example_round_trip>`
  - :ref:`Example - Custom Caustic Function <py_example_custom_caustic>`
