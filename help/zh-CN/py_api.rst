.. _py_api:
.. index:: single: python api

Python API
==========

|rezonator| 提供了一个 Python API，允许你编写自定义的计算和绘图函数。这些函数可以访问光路元件、执行计算，并生成自定义的表格和绘图。API 提供了两个主要模块：:doc:`rezonator <py_global>`（为简便起见别名为 ``Z``）和 :doc:`schema <py_schema>`。

.. tocTree::
   :maxDepth: 1
   :caption: API 模块和类：

   py_global
   py_schema
   py_element
   py_matrix
   py_matrix3
   py_param_ref
   py_ray_vector
   py_round_trip
   py_examples

重要说明
---------------

元件索引
~~~~~~~~~~~~~~~~

与普通的 Python 列表不同，光路中的元件从 1 开始索引，以便与它们在 :doc:`schema_elems` 中的编号方式保持一致。

测量单位
~~~~~~~~~~~~~~~~~

所有值都采用 SI 单位（米、弧度等）。在文本输出中显示时，按需要对它们进行换算。对于 :doc:`table <custom_table>` 和 :doc:`plotting <custom_plot>` 函数则无需换算，因为 :doc:`table <table_window>` 和 :doc:`plot <plot_window>` 窗口都允许选择合适的测量单位来显示数值。

临时更改
~~~~~~~~~~~~~~~~~

在对参数进行临时 :ref:`更改 <py_method_elem_set_param>` 时（例如在 :doc:`plot function <custom_plot>` 的计算过程中），使用 :ref:`Element.lock() <py_method_elem_lock>` 和 :ref:`Element.unlock() <py_method_elem_unlock>` 方法（或者处理全局参数的相应 :ref:`schema.param_lock() <py_method_schema_lock_param>` 和 :ref:`schema.param_unlock() <py_method_schema_unlock_param>`），以确保正确恢复，并避免界面进行不必要的更新。

更方便的方式是使用 Python 的 ``with`` 块以及专门的 :ref:`Element.param_ref() <py_method_elem_param_ref>` 或 :ref:`schema.param_ref() <py_method_schema_param_ref>` 方法，它们返回 :doc:`ParamRef <py_param_ref>` 对象，可以自动锁定/解锁并恢复参数值。

性能
~~~~~~~~~~~

在每次计算光束属性之前（例如 :ref:`RoundTrip.beam_radius() <py_method_rt_beam_radius>`），都会自动进行 :ref:`Round-trip matrix <round_trip>` 的计算。这保证了计算会考虑元件的实际参数。对于同一属性的多次查询，应将结果存入局部变量，而不是重复计算。

.. seeAlso::

  - :doc:`functions`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
