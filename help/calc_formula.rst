.. index:: single: formula calculator

Formula Calculator
==================

:menuselection:`Tools --> Formula Calculator`

Formula Calculator is the tool allowing computation of custom formulas given as text expression. Enter your formula in the text field :guipart:`1` and hit :kbd:`Ctrl+Enter` or press :guilabel:`Calculate` button on the toolbar. Results of calculation are displayed in field :guipart:`2`.

|rezonator| uses `Lua <http://www.lua.org>`_ internally to calculate custom formulas. See :doc:`lua_primer` to discover supported operations and functions.

    .. image:: img/calc_formula.png

In case of simple formula you can type it as it is, e.g.::

    sin(deg2rad(30))

For more complex formulas, it is possible to define additional variables to make the expression clearer and easily understandable, e.g.::

    L = 2.5
    n = 1.33
    alpha = deg2rad(30)
    ans = L / sqrt(n^2 - sin(alpha)^2)

In this case, you have to specify explicitly what value should be treated as the final result. Store the result value in the predefined variable ``ans`` (stand for 'answer'). All variables, including ``ans``, listed in the table :guipart:`3` and can be reused in subsequent calculations. 


    .. note::

        You can not just define a variable and not provide an answer, so the expression like ``a = 2`` is invalid. If you only want to assign a variable, use the variable as the answer: ``a = 2; ans = a``.

.. seealso::

    :doc:`lua_primer`