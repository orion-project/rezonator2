.. _elem_opers:

Element Operations
==================

If the schema window is active, the :menuSelection:`Element` item appears in the menu bar of the project window. This contains basic commands to operate with elements. Some of these commands are also collected in the context menu of both the :doc:`schema_elems` and :doc:`layout`.

.. --------------------------------------------------------------------------

Append
------

.. _elem_opers_append:

:menuSelection:`Element --> Append... (Ctrl+Ins)`

The command opens the :doc:`catalog` to choose a type for a new element. The newly created element gets inserted into the position of the current selection in the :doc:`schema_elems`. A :ref:`label <elem_props_label>` is automatically generated for the new element if this is allowed by the :ref:`application settings <app_settings_gen_labels_created>`. The :doc:`elem_props` dialog is automatically opened right after the element has been inserted to provide parameter values (if this is allowed by the :ref:`application settings <app_settings_edit_just _created>`).

.. --------------------------------------------------------------------------

Replace
-------

:menuSelection:`Element --> Replace...`

The command opens the :doc:`catalog` to choose a type for the currently selected element. The current element gets replaced with an element of the new type, and its properties are copied to the new element. Parameters are copied by name, so, for example, if you replace a :doc:`matrix/ElemCurveMirror` with a :doc:`matrix/ElemThinLens`, the angle will be copied because both of them have the parameter :param:`α`. While the focal range will not be copied because they are named differently and the application doesn't include any heuristics to say the :param:`R` and :param:`F` mean almost the same.

.. --------------------------------------------------------------------------

Move Selected
~~~~~~~~~~~~~

:menuSelection:`Element --> Selected Up`

:menuSelection:`Element --> Selected Down`

Move the selected element up or down over the list.

  .. note:: Moving of several element is not supported. So don't use multi-selection with the command to avoid confusions.

.. --------------------------------------------------------------------------

Properties
~~~~~~~~~~

:menuSelection:`Element --> Properties... (Return)`

Open the :doc:`elem_props` dialog to set element parameters. After an element has been edited, all opened windows with :ref:`calculation results <functions>` (diagrams, tables, etc.) are refreshed automatically. Exception is :ref:`frozen <func_freeze>` calculation results. 

.. --------------------------------------------------------------------------

.. _elem_show_matrix:

Show Matrix
-----------

:menuSelection:`Element --> Show Matrix (Shift+Return)`

The command shows the ray matrix of one or several selected elements. Matrices are displayed in a special :ref:`information window <info_window>`. 

.. --------------------------------------------------------------------------

.. _elem_opers_add_adjuster:

Add Adjuster
------------

:menuSelection:`Context menu --> Add Adjuster`

The command allows for adding a new parameter adjuster of the selected element to an active (or new if it is not yet) schema :ref:`adjustment window <adjust>`. If there are several adjustable parameters available, the command displays a pulldown menu where it is necessary to choose a parameter to adjust. 

  .. image:: img/elem_opers_adjust.png

.. --------------------------------------------------------------------------

Copy
----

:menuSelection:`Edit --> Copy (Ctrl+C)`

The command copies the currently selected element into the clipboard in an internal format. The copied elements can be pasted into another schema or they also can be pasted into the :doc:`elem_library` to be used later as presets.

.. --------------------------------------------------------------------------

.. _elem_opers_paste:

Paste
-----

:menuSelection:`Edit --> Paste (Ctrl+V)`

The commands insert previously copied element into the current position of the :doc:`schema_elems`.

When pasting, a new :ref:`label <elem_props_label>` can be generated for pasted elements if this is allowed by the :ref:`application settings <app_settings_gen_labels_pasted>`.

Element presets can also be pasted from the :doc:`elem_library`.

.. --------------------------------------------------------------------------

.. _elem_toggle_lock:

Disable/Enable
--------------

:menuSelection:`Element --> Disable/Enable`

The command toggles the :ref:`enabled flag <elem_props_lock>` of one or several elements. The main purpose of having this as a separate command is to quickly switch an element type. For example, you can create two elements and disable one of them. Then select both elements and hit the command, the element change their states simultaneously - that one that has been disabled gets enabled and the second one gets disabled. This allows for quick switching between element and checking what changes.

  .. image:: img/elem_opers_toggle_lock_1.png

  .. image:: img/elem_opers_toggle_lock_2.png

.. --------------------------------------------------------------------------

Delete
------

:menuSelection:`Element --> Delete... (Ctrl+Del)`

The command deletes one or several elements that are currently selected in the :doc:`schema_elems`.

.. --------------------------------------------------------------------------

.. _elem_save_custom_lib:

Save to Custom Library
----------------------

:menuSelection:`Element --> Save to Custom Library...`

Save the selected element as a preset into the :doc:`elem_library`. Current element parameter values are used for the preset. If some parameters are linked to :ref:`global parameters <params_window>`, only the parameter value is stored in the library, not the link. Because global parameters are related to the current schema but the library is not.

.. --------------------------------------------------------------------------

.. seeAlso::

  :doc:`schema_window`
