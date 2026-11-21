.. _elem_library:

自定义元件库
=======================

:menuSelection:`Tools --> Custom Elements Library`

  .. image:: img/elem_library.png

元件库包含元件预设。它们是预先赋予了参数值和属性的元件。自定义预设显示在 :doc:`catalog` 的 “Custom library” 选项卡上。可以像往常一样从那里把它们添加到光路中。添加到光路的是预设的一个副本，所有参数值都会赋给目标元件。预设的标签用作新元件的*标签前缀*（见 :ref:`elem_props_label`、:ref:`app_settings_gen_labels_created`）。

使用 :menuSelection:`Append` 按钮打开 :doc:`catalog` 并向库中添加一个预设。或者，你也可以在光路窗口中使用命令 :ref:`elem_save_custom_lib`，从已有元件创建一个预设。此外，你还可以使用库窗口中的 :menuSelection:`Copy` 命令和光路窗口中的 :ref:`Paste <elem_opers_paste>` 命令。在这种情况下，它的工作方式与通常的粘贴操作相同，预设标签不会用作标签前缀（见 :ref:`app_settings_gen_labels_pasted`）。

使用工具栏上的 :menuSelection:`Properties` 按钮更改预设参数。这一更改不会影响已经由该预设创建的元件。

使用工具栏上的 :menuSelection:`Delete` 按钮将预设从库和 :doc:`catalog` 中删除。这不会影响已经由该预设创建的元件。

.. note::
  自定义库存储在本地用户的个人配置中，因此不会影响同一系统中的其他用户。

