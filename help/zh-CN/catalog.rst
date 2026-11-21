.. _catalog:

元件目录
================

:menuSelection:`Element --> Append`

  .. image:: img/catalog.png

元件目录窗口包含一个按类别划分的元件类型列表，如 “Common”、“Interfaces” 等等。每种元件类型都有自己的小示意图（图标），用于在光路的 :ref:`元件列表 <schema_elems>` 中标识该元件的类型。此外，每种元件类型都有一幅较大的轮廓图，用作参考信息，以展示元件参数的含义。

.. _catalog_custom_lib:

自定义库
--------------

目录对话框中还可能有 “Custom library” 选项卡，显示带有预设参数的元件预设列表。它们可以像往常一样添加到光路中。添加到光路的是预设的一个副本，所有参数值都会赋给目标元件。预设的标签用作新元件的*标签前缀*（见 :ref:`elem_props_label`、:ref:`app_settings_gen_labels_created`）。使用 :doc:`elem_library` 窗口管理这些预设。

  .. image:: img/catalog_custom.png

.. seeAlso::

    - :doc:`elem_matrs`
    - :doc:`elem_props`
    - :doc:`elem_library`
