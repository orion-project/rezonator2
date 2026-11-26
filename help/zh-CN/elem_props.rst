.. _elem_props:

元件属性
==================

:menuSelection:`Element --> Properties`

通用属性
-----------------

  .. image:: img/elem_props_common.png

.. _elem_props_label:

标签
~~~~~

它是元件的标识符或标签。标签通常由几个字母组成，例如 `M1` 或 `L_foc`。这就是元件在光学图纸上通常的标注方式。

如果相应的 :ref:`应用选项 <app_settings_gen_labels_created>` 允许，标签可以在创建元件时自动生成。每种 :ref:`元件类型 <elem_matrs>` 都有自己的标签前缀。标签由该前缀加上一个递增的索引组成。例如，距离的标签格式为 `L1`、`L2`……，反射镜的标签格式为 `M1`、`M2`……。


.. _elem_props_title:

标题
~~~~~

它是元件的名称或其简短描述。例如 “Output mirror”。该字段完全是信息性的。


属性选项卡
-------------

还有一组附加属性，它们因元件类型而异。

.. tocTree::
  :maxDepth: 1

  elem_props_params
  elem_props_options
  elem_props_outline
  custom_params
