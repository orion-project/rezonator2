.. index:: single: Element (Python)

类：Element
==============

表示光路中的一个光学 :doc:`element <elements>`。

.. note::

  Element 对象不能直接创建；使用 :ref:`schema.elem() <py_method_schema_elem>` 函数来获取它们。

属性
----------

``label`` (str, read-only)
  元件的 :ref:`label <elem_props_label>`。

``index`` (int, read-only)
  元件在光路中从 1 开始的索引（如果未找到则为 None）。与普通的 Python 列表不同，光路中的元件从 1 开始索引，以便与它们在 :doc:`schema_elems` 中的编号方式保持一致。

``length`` (float, read-only)
    以米为单位的长度参数（如果元件没有长度则为 None）。

``axial_length`` (float, read-only)
    以米为单位的 :ref:`Axial length <axial_length>`（如果元件没有长度则为 None）。

``optical_path`` (float, read-only)
    以米为单位的光程长度（如果元件没有长度则为 None）。

``ior`` (float, read-only)
    折射率（如果不适用则为 None）。

``offset`` (float, read/write)
    以米为单位的元件内部偏移量（如果元件没有长度则为 None）。当往返以 ``inside=True`` :ref:`calculated <calc_round_trip_subrange>` 时，偏移值用于具有长度的元件。参见 :ref:`schema.round_trip() <py_method_schema_round_trip>`。

``prev`` (Element, read-only)
    遵循 :ref:`round-trip <round_trip>` 规则的上一个元件。该属性还会忽略 :ref:`disabled <elem_props_lock>` 元件，因为它们不参与往返。

``next`` (Element, read-only)
    遵循 :ref:`round-trip <round_trip>` 规则的下一个元件。该属性还会忽略 :ref:`disabled <elem_props_lock>` 元件，因为它们不参与往返。

``disabled`` (bool, read-only)
    该元件是否被 :ref:`disabled <elem_props_lock>`。

.. #######################################################################

方法
-------

.. index:: single: param (Python)
.. _py_method_elem_param:

``param(label, default=None)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

按标签获取元件参数值。此方法既可以访问元件的预定义参数，也可以访问 :ref:`custom <custom_params>` 参数。

参数：

- ``label`` (str) - 参数标签
- ``default`` (float, optional) - 如果未找到参数时的默认值

返回值：

- float - 以 SI 单位表示的参数值，如果未找到则为 None/default

示例：

.. code-block:: python

    elem = schema.elem('F1')
    f = elem.param('F')  # Get focal length
    r = elem.param('dYt', 0.0)  # Get axial misalignment, default to 0 if not found

.. #######################################################################

.. index:: single: set_param (Python)
.. _py_method_elem_set_param:

``set_param(label, value)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

按标签设置元件参数值。

参数：

- ``label`` (str) - 参数标签
- ``value`` (float) - 以 SI 单位表示的参数值

引发：

- ``KeyError`` - 如果未找到该参数
- ``ValueError`` - 如果值无效

示例：

.. code-block:: python

    elem = schema.elem('L1')
    elem.set_param('F', 0.1)  # Set focal length to 100 mm

.. #######################################################################

.. index:: single: matrix (Python)
.. _py_method_elem_matrix:

``matrix(plane)``
~~~~~~~~~~~~~~~~~

返回元件在给定平面内的矩阵；如果未提供参数，则返回 T 平面内的矩阵。

参数：

- ``plane`` - :ref:`Work plane <work_plane>`（'T'、'S'、:ref:`Z.PLANE_T <py_const_plane_t>` 或 :ref:`Z.PLANE_S <py_const_plane_s>`）

引发：

- ``ValueError`` - 如果平面名称无效


.. #######################################################################

.. index:: single: lock (Python)
.. _py_method_elem_lock:

``lock()``
~~~~~~~~~~

禁用元件事件并备份参数值。在进行临时更改时使用。

.. #######################################################################

.. index:: single: unlock (Python)
.. _py_method_elem_unlock:

``unlock()``
~~~~~~~~~~~~

启用元件事件并恢复已备份的参数值。

.. note::

  在对参数进行临时更改时（例如在 :doc:`plot function <custom_plot>` 的计算过程中），使用 ``Element.lock()`` 和 ``Element.unlock()`` 方法，以确保正确恢复，并避免界面进行不必要的更新。如果界面更新过于频繁，会显著拖慢应用程序。

示例：

.. code-block:: python

  elem = schema.elem('Cr')
  elem.lock()
  try:
    # Make temporary changes:
    elem.length = elem.length * 1.1

    # Calculate something...
  finally:
    # Restore original values
    elem.unlock()

.. #######################################################################

.. index:: single: param_ref (Python)
.. _py_method_elem_param_ref:

``param_ref(label)``
~~~~~~~~~~~~~~~~~~~~

创建一个 :doc:`ParamRef <py_param_ref>` 对象，以便在绘图函数中更方便地实现锁定-解锁模式。

参数：

- ``label`` (str) - 参数标签

引发：

- ``KeyError`` - 如果未找到该参数

示例：

.. code-block:: python

    elem = schema.elem('Cr')
    with elem.param_ref('L') as p:
      # Make temporary changes:
      for i in range(num_points):
        p.set_value(i)

        # Calculate something...

    #  Original value is automatically restored


.. seeAlso::

  - :doc:`py_api`
  - :doc:`elem_props`
  - :ref:`Example - Basic Element Manipulation <py_example_basic_elem_manipulation>`
