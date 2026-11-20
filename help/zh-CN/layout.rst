.. _layout:

布局
======

光路布局是光路自动生成的示意性表示，采用线性视图，即忽略光轴的所有弯折，元件显示在一行中。

这是 “z_crystal” 示例的布局。它展示了 Z 形谐振腔如何展开为线性视图：

  .. image:: img/layout.png

每种元件类型在布局上都有自己的表示方式。注意，如果球面镜没有放在光路的末端，它们会显示为透镜。如果光束以一定角度入射到反射镜或透镜上，相应的元件会在布局上倾斜显示，以表明系统存在像散。

  .. note:: 与 :doc:`schema_elems` 不同，被 :ref:`禁用 <elem_props_lock>` 的元件不会显示在布局上。

功能
--------

交互性
~~~~~~~~~~~~~

布局是可交互的。你可以点击一个元件来选中它。按住 :menuSelection:`Ctrl` 键可以选中多个元件。布局中的选择与 :doc:`schema_elems` 中的选择是同步的。当在布局上选中一个元件时，上下文菜单中会包含一些针对元件的 :ref:`命令 <elem_opers>`。当你点击空白处时，上下文菜单中会出现 :menuSelection:`Copy Image` 命令。它将布局内容作为图像复制到剪贴板。

可选标签
~~~~~~~~~~~~~~~

元件 :ref:`标签 <elem_props_label>` 显示在元件上方。为了获得更好的视图，你可以使用相应的 :ref:`元件选项 <elem_props_show_label>` 隐藏某些标签。

  .. image:: img/layout_hide_labels.png

窄视图
~~~~~~~~~~~

元件长度在布局上不按比例显示。它们都有固定的长度。为了获得更好的视图，某些元件类型，如 :doc:`matrix/ElemEmptyRange` 或 :doc:`matrix/ElemTiltedInterface`，可以有更窄的表示方式。这可以通过另一个 :ref:`元件选项 <elem_props_draw_narrow>` 启用。

  .. image:: img/layout_narrow_elem.png

.. seeAlso::

  - :doc:`schema_elems`
