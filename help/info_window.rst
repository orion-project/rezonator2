.. _info_window:
.. index:: single: info window

Information Functions
=====================

Results of calculations of certain functions are one or several values. Such functions are called information functions. They usually do not require any parameters. Examples are functions :doc:`func_rt`, :doc:`func_reprate`. 

Results of information functions are represented in a textual form in small popup windows.

  .. image:: img/info_window.png

.. note::
  Unlike :ref:`plot <plot_window>` or :ref:`table <table_window>` windows, information windows are not saved in the schema project file.

Commands
--------

Information windows do not show a menu in the main window, so their commands are only available on the local window toolbar or via hotkeys.

.. --------------------------------------------------------------------------

Update
~~~~~~

:menuSelection:`F5`

The command calculates the function again. The command is inaccessible if the current function is :ref:`frozen <func_freeze>`.

.. --------------------------------------------------------------------------

Freeze
~~~~~~

:menuSelection:`Ctrl+F`

The command freezes the current information window. See :doc:`func_freeze` for details. 

.. --------------------------------------------------------------------------

Frozen Info
~~~~~~~~~~~

The button displays a small popup showing element parameter values at the moment where the function had been :ref:`frozen <func_freeze>`.

.. --------------------------------------------------------------------------

Copy
~~~~

:menuSelection:`Ctrl+C`

The command copies the selected content as a formatted HTML text into the clipboard.

.. --------------------------------------------------------------------------

Copy All
~~~~~~~~

The command copies the all content as a formatted HTML text into the clipboard. You don't need to have a selection in the editor for this command.

.. --------------------------------------------------------------------------

Help
~~~~

:menuSelection:`F1`

The command shows a help topic associated with the current function.

.. --------------------------------------------------------------------------

Additional Commands
~~~~~~~~~~~~~~~~~~~

Some functions (e.g. :doc:`func_rt`) can add more buttons to the toolbar that are specific to that type of function. They are described in the particular function help topic.

.. --------------------------------------------------------------------------

.. seeAlso::

  :doc:`functions`, :doc:`plot_window`, :doc:`table_window`
