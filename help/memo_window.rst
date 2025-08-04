Memo Window
===========

:menuSelection:`Window --> Memos`

  .. image:: img/memo_window.png

The memo window is a simple rich text editor where you can take work notes, keep some pieces of useful information, and even paste images into it, and all these data will be stored in your schema project file.

Some example files, e.g. “v_crystal” or “z_crystal”, already include sample memo content showing a schema representation in traditional view (unlike the linear view used in the :doc:`layout`).

The memo window can be safely closed; its content will still be kept in the schema. Use the :menuSelection:`Window --> Memos` command or the :menuSelection:`Memos` button on the main toolbar to show the content again.

Indicators
----------

When there is a memo in a schema, the :menuSelection:`Memos` button on the main toolbar displays a red badge showing 1 (currently, there is only one memo that could be in the project):

  .. image:: img/memo_window_badge.png

Commands
--------

When the memo window is active, the :menuSelection:`Memo` item appears in the menu bar of the project window. Besides of the standard text formatting commands that also available on the toolbar it contains some additional commands.

Insert Table
~~~~~~~~~~~~

:menuSelection:`Memo --> Insert Table...`

The command shows the dialog to create a table with fixed numbers of rows and columns.

  .. note:: Tables are very simplistic in the editor, so further changing of row and column count is not supported. Table formatting (link frame color or margins) is also not supported.

Export as PDF
~~~~~~~~~~~~~

:menuSelection:`Memo --> Export as PDF...`

Export the current memo content into a PDF file.
