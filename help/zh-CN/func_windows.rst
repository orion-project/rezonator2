.. _func_windows:

****************
函数窗口
****************

.. tocTree::
   :maxDepth: 1

   plot_window
   plot_opers
   plot_cursor
   table_window
   table_opers
   table_symbols
   info_window
   func_freeze

任何 :ref:`函数 <functions>` 的计算结果都呈现在所谓的函数窗口中。所有函数及其窗口根据所得结果的视图分为几种类型。函数类型有 :ref:`绘图 <plot_window>`、:ref:`表格 <table_window>` 和 :ref:`信息 <info_window>`。

.. TODO: Exception are ‘frozen’ functions which data are saved into schema file. You can use destined program preferences to disable saving of function windows into schema file or to disable restoring of these when schema file are loaded. 

所有已打开的函数窗口列表及其设置会保存到光路项目文件中，但计算结果不会保存。加载光路文件时，已保存的函数窗口会被恢复，函数会被重新计算。因此，已保存的函数并不包含真实数据，只包含对函数的引用——数据正是借助这些函数得到的。

  .. image:: img/func_windows.png
