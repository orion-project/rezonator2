.. _func_freeze:
.. index:: single: freezing

函数冻结
=================

:menuSelection:`Plot --> Freeze`

:menuSelection:`Table --> Freeze`

默认情况下，当你更改光路或某些元件的参数时，大多数函数窗口的内容（函数结果）会自动刷新。冻结会禁用某个函数的这种自动重新计算。

**使用示例：** 假设你有一个 :doc:`二维稳定性图<func_stabmap_2d>`，并且一直在通过研究该图为谐振腔选择工作条件。你反复更改某个元件的一个参数（通过直接编辑 :doc:`元件参数<elem_props>`，或借助 :doc:`微调<adjust>`），而该参数正是稳定性图的可变参数（自变量）之一。同时你观察例如某个元件内部的 :doc:`焦散<func_caustic>` 在你更改参数值时如何变化。稳定性图每次也会被重新计算，但由于你更改的是它的可变参数，它的外观显然保持不变。如果二维稳定性图绘制得比较精细，重复计算它可能需要很长时间。在这种情况下，你可以冻结稳定性图窗口，以禁用对它不必要的重新计算。

函数冻结可以借助函数窗口工具栏上的相应按钮完成，也可以通过函数窗口菜单中的 :menuSelection:`Freeze` 命令完成。当一个函数被冻结时，函数窗口工具栏上会出现一个特殊按钮。它称为 “Frozen Info”，其中包含冻结时刻所有元件的参数值信息，即所谓的“冻结信息”。

    .. image:: img/func_freeze.png

.. TODO: When schema is saved, the calculated data of frozen function widows and frozen information are saved into schema file. When such file is loaded into resonator, frozen functions are not calculated but its frozen data are just displayed.

.. TODO: Note:  not all functions provide saving of frozen data in current version of program. Help topic for certain function informs if function does not provide this ability. In such case saved frozen function windows will be empty (containing no data) after schema file will be loaded.

    .. note::
        目前，冻结状态不会保存到光路项目文件中。因此当你重新打开项目时，所有被冻结的函数都会像往常一样被重新计算并显示。

.. seeAlso::

    - :doc:`functions`
