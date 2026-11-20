备忘窗口
===========

:menuSelection:`Window --> Memos`

  .. image:: img/memo_window.png

备忘窗口是一个简单的富文本编辑器，你可以在其中记录工作笔记、保存一些有用的信息，甚至可以将图像粘贴到其中，所有这些数据都会存储在你的光路项目文件中。

某些示例文件，例如 “v_crystal” 或 “z_crystal”，已经包含了示例备忘内容，以传统视图显示光路表示（与 :doc:`layout` 中使用的线性视图不同）。

备忘窗口可以安全关闭；其内容仍会保存在光路中。使用 :menuSelection:`Window --> Memos` 命令或主工具栏上的 :menuSelection:`Memos` 按钮可以再次显示内容。

指示标记
----------

当光路中有一条备忘时，主工具栏上的 :menuSelection:`Memos` 按钮会显示一个红色徽标，上面显示 1（目前，项目中只能有一条备忘）：

  .. image:: img/memo_window_badge.png

命令
--------

当备忘窗口处于活动状态时，项目窗口的菜单栏中会出现 :menuSelection:`Memo` 项。除了工具栏上也有的标准文本格式化命令外，它还包含一些附加命令。

插入表格
~~~~~~~~~~~~

:menuSelection:`Memo --> Insert Table...`

该命令显示用于创建具有固定行数和列数的表格的对话框。

  .. note:: 编辑器中的表格非常简单，因此不支持进一步更改行数和列数。也不支持表格格式化（链接边框颜色或边距）。

导出为 PDF
~~~~~~~~~~~~~

:menuSelection:`Memo --> Export as PDF...`

将当前备忘内容导出到一个 PDF 文件。
