.. index:: single: ParamRef (Python)

类：ParamRef
===============

对全局参数或元件参数的引用，它针对多次更改参数值并恢复其原始值进行了优化，这正是绘图函数所需要的。

此类对象不能直接构造；应使用 :ref:`schema.param_ref() <py_method_schema_param_ref>` 或 :ref:`Element.param_ref() <py_method_elem_param_ref>` 函数来获取它们。

此类旨在与 Python 的 ``with`` 语句一起使用。当执行进入该块时，原始参数值会被记住，所有直接或间接依赖于该参数的元件都会被 :ref:`locked <custom_plot_lock>`，以防止在修改参数值时出现不必要的界面变化。当执行退出该块后，原始参数值会被恢复，所有被锁定的元件都会被解锁。这简化了绘图过程，也不必让你记住显式调用锁定/解锁方法。

典型的使用场景如下

.. code-block:: python

    # Prepare arrays for store graph points
    graph_x = []
    graph_y = []
    # Prepare a round trip once,
    # no need to rebuild it for each point
    rt = schema.round_trip()
    with schema.param_ref('L_foc') as L_foc:
      for i in range(num_points):
        # Calculate the next argument
        x = L_foc_min + L_foc_step * i
        L_foc.set_value(x)
        # Calculate the next plot point
        y = rt.beam_radius()
        graph_x.append(x)
        graph_y.append(y)

参见 ``global_params_py_set.rez`` 示例，了解它的实际用法。

方法
-------

.. index:: single: value (Python)
.. _py_method_param_ref_value:

``value()``
~~~~~~~~~~~

返回以 SI 单位表示的参数值。

.. #######################################################################

.. index:: single: set_value (Python)
.. _py_method_param_ref_set_value:

``set_value(value)``
~~~~~~~~~~~~~~~~~~~~

设置参数值。

参数：

- ``value`` (float) - 以 SI 单位表示的参数值

引发：

- ``ValueError`` - 如果值无效

.. seeAlso::

  - :doc:`custom_plot`
