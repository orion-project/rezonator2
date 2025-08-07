.. _func_windows:

****************
Function Windows
****************

.. tocTree::
   :maxDepth: 1

   plot_window
   plot_cursor
   table_window
   table_symbols
   info_window
   func_freeze

Results of calculations of any :ref:`functions <functions>` are presented in so-called function windows. All functions and their windows are divided into several types based on the view of obtained results. Function types are :ref:`plot <plot_window>`, :ref:`table <table_window>`, and :ref:`information <info_window>`.

.. TODO: Exception are ‘frozen’ functions which data are saved into schema file. You can use destined program preferences to disable saving of function windows into schema file or to disable restoring of these when schema file are loaded. 

A list of all opened function windows and their settings is saved into the schema project file, but the results of calculations are not. Saved function windows are restored and functions are recalculated again when you load a schema file. Therefore, saved functions do not contain real data but only references to functions with the help of which those data have been obtained.

  .. image:: img/func_windows.png
