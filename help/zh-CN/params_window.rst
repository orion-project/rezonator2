.. _params_window:

参数窗口
=============

.. index:: single: global parameter
.. index:: single: schema parameter

:menuSelection:`Window --> Parameters (F11)`

一个光路工程可以保存若干独立于元件的参数，因此它们被称为 “全局参数”。它们的值可以直接赋值，也可以通过公式依赖于其他参数来表示（见下文）。

全局参数可用于为 :ref:`元件参数 <elem_props_link>` 提供值，这使得光路中的元件能够相互关联、相互驱动。然后你可以更改某个全局参数的值，这会立即同时影响几个元件。

  .. image:: img/params_window.png

.. --------------------------------------------------------------------------

.. _params_window_create:

创建新参数
--------------------

:menuSelection:`Parameter --> Create... (Ctrl+Ins)`

创建新参数时，应当指定它的 Name、Dimension 和 Description。计量单位可以在以后指定参数量值时再设置。

  .. image:: img/params_window_create.png

.. --------------------------------------------------------------------------

.. _params_window_edit:

编辑参数
------------------

:menuSelection:`Parameter --> Edit... (Ctrl+Return)`

编辑参数时，可以更改它的名称。然后该名称会在使用这个参数的元件和公式中自动更新。

也可以更改参数的量纲。在这种情况下，如果参数被用在了错误的上下文中，其责任由用户承担。例如，如果你把 F0 参数的量纲改为 “Angular”，那么焦距与 F0 关联的透镜元件很可能会取到不正确的值。当量纲不匹配时，参数值会通过 SI 换算来赋值。例如，假设与 F0 关联的焦距以 cm 显示，把 F0 从 “Linear” 改为 “Angular”，我们会得到像 `10deg --> 0.1745rad --> 0.1745m --> 17.45cm` 这样的变换。

Description 只是参数的一个简短的、便于人阅读的说明，它的更改只影响表中显示的内容。

  .. image:: img/params_window_edit.png

.. --------------------------------------------------------------------------

.. _params_window_value:

设置参数值
-------------------

:menuSelection:`Parameter --> Set... (Return)`

参数值可以通过对话框直接赋值。

  .. image:: img/params_window_set.png


参数值也可以通过简单表达式给出。表达式只能包含数字和基本的数学函数（见 :doc:`lua_primer`）。它不能引用其他参数。如果你需要引用，应当改用公式，见下文。当给出表达式时，当你把鼠标悬停在输入框上时，实际的参数值会显示在工具提示中。

  .. image:: img/params_window_set_expr.png

.. --------------------------------------------------------------------------

.. _params_window_formula:

设置参数公式
---------------------

:menuSelection:`Parameter --> Set... (Return) --> Add formula`

参数值可以由其他参数的值导出。你可以提供一个用于计算值的公式。在 “Set value” 对话框中，点击右侧的条形菜单并选择 :menuSelection:`Add formula` 项。以后，如果你不再需要公式、想直接提供参数值，可以使用同一个菜单和命令 :menuSelection:`Remove formula`。

  .. image:: img/params_window_formula_1.png

这时会出现一个对话框，你可以在其中输入一个表达式，它将被计算以得到参数值。公式用 Lua 语法编写（见 :doc:`lua_primer`）。在你输入时，公式会被即时检查是否正确，如果有问题会显示一个红色标记。

  .. image:: img/params_window_formula_2.png

公式可以依赖于其他参数的值。只需输入另一个参数的名称，依赖关系就会自动创建。在图中，:param:`L0` 和 :param:`F0` 是作为依赖项添加的其他参数的名称。当其中一个依赖项改变时，被驱动的参数会根据新值自动重新计算。

当一个参数由公式驱动时，它在表中用 “齿轮” 图标标记，依赖项列表显示在 “Name” 列中。例如 :param:`L1 = f(L0, F0)` 表示参数 :param:`L1` 的值是由参数 :param:`L0` 和 :param:`F0` 的值以某种方式计算出来的，它们也都在这个表中。

.. --------------------------------------------------------------------------

.. _params_window_adjust:

调节参数
----------------

:menuSelection:`Parameter --> Adjust`

全局参数可以像任何其他元件参数一样，在 :doc:`调节工具 <adjust>` 中使用。只需从参数表的上下文菜单中选择 :menuSelection:`Adjust` 命令，或使用窗口菜单中的 :menuSelection:`Adjust` 命令。但是，如果参数由公式驱动，它就不能真正被调节，因为你不能直接给被驱动的参数提供值（注意图中 :param:`L1` 的值以斜体字标记）。相反，你必须调节它的依赖项，调节工具会反映被驱动的值是如何变化的。

  .. image:: img/params_window_adjust.png

.. note::
  如果曾为参数值设置过表达式，那么在使用调节工具后，它会被重置并替换为实际值。因为调节工具无法知道表达式的哪一部分应该被改变，所以它改变的是整个值。

删除参数
-----------------

:menuSelection:`Parameter --> Delete (Ctrl+Del)`

系统会检查要删除的参数是否是另一个全局参数的依赖项，或者是否有元件参数与它关联，并阻止删除这样的参数。所有依赖关系和关联都应当手动解除。
