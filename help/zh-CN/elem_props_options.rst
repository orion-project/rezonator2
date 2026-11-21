.. _elem_props_options:

元件属性：选项
===========================

:menuSelection:`Element --> Properties --> Options`

  .. image:: img/elem_props_options.png

.. --------------------------------------------------------------------------

.. _elem_props_lock:

在计算中忽略（禁用元件）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当该标志被设置时，该元件在往返计算中被忽略，并且不会出现在函数参数选择列表中。看起来就像该元件被临时从光路中删除了一样。

  .. image:: img/elems_table_locked.png

此外，这个标志也可以通过 :doc:`schema_window` 中的 :ref:`单独命令 <elem_toggle_lock>` 快速切换。

.. --------------------------------------------------------------------------

.. _elem_props_show_label:

在布局上显示元件标签
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

该选项定义是否在 :doc:`layout` 上显示元件标签。这是一个外观选项，允许你为包含某些类型元件（如 :ref:`界面 <ElemNormalInterface>` 或 :ref:`点 <ElemPoint>`）的布局获得更好的视图。

例如，这里是 “singlet_thick_intf” 示例的布局，其中表示透镜表面的 :doc:`matrix/ElemSphericalInterface` 元件的标签被隐藏了：

  .. image:: img/layout_hide_labels.png

.. --------------------------------------------------------------------------

.. _elem_props_draw_narrow:

绘制元件的窄版形式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

某些元件类型，如 :doc:`matrix/ElemEmptyRange` 或 :doc:`matrix/ElemTiltedInterface`，可以在 :doc:`layout` 上有更窄的表示形式。这是一个外观选项，允许你为包含某些类型元件的布局获得更好的视图。

例如，这里是 “z_crystal” 示例的布局，其中 :param:`L3` 以窄模式显示，这反映了它明显短于 :param:`L2` 距离这一事实：

  .. image:: img/layout_narrow_elem.png

.. --------------------------------------------------------------------------

.. seeAlso::

    - :doc:`elem_props`
    - :ref:`元件属性：参数 <elem_props_params>`
    - :ref:`元件属性：轮廓 <elem_props_outline>`
