.. index:: single: formula calculator

公式计算器
==================

:menuselection:`Tools --> Formula Calculator`

公式计算器是一个允许计算以文本表达式形式给出的自定义公式的工具。在文本字段 :guipart:`1` 中输入公式，然后按 :kbd:`Ctrl+Enter` 或按工具栏上的 :guilabel:`Calculate` 按钮。计算结果显示在字段 :guipart:`2` 中。

|rezonator| 内部使用 `Lua <http://www.lua.org>`_ 来计算自定义公式。参见 :doc:`lua_primer`，了解支持的运算和函数。

    .. image:: img/calc_formula.png

对于简单的公式，你可以原样输入，例如::

    sin(deg2rad(30))

对于更复杂的公式，可以定义附加变量，让表达式更清晰易懂，例如::

    L = 2.5
    n = 1.33
    alpha = deg2rad(30)
    ans = L / sqrt(n^2 - sin(alpha)^2)

在这种情况下，你必须明确指定哪个值应被视为最终结果。把结果值存入预定义变量 ``ans``（即 “answer”）。所有变量（包括 ``ans``）都列在表 :guipart:`3` 中，可以在后续计算中重复使用。


    .. note::

        你不能只定义一个变量而不提供答案，所以像 ``a = 2`` 这样的表达式是无效的。如果你只想给一个变量赋值，就把该变量用作答案：``a = 2; ans = a``。

.. seealso::

    :doc:`lua_primer`
