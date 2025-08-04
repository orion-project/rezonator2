.. _elem_library:

Custom Elements Library
=======================

:menuSelection:`Tools --> Custom Elements Library`

  .. image:: img/elem_library.png

The Elements library contains element presets. They are elements having preassigned parameter values and properties. Custom presets are displayed on the "Custom library" tab in the :doc:`catalog`. They can be added to the schema from there as usual. A copy of the preset is added to the schema, and all parameter values get assigned to the target element. The label of the preset is used as the *label prefix* for the new elements (see :ref:`elem_props_label`, :ref:`app_settings_gen_labels_created`).

Use the :menuSelection:`Append` button to open the :doc:`catalog` and add a preset to the library. Alternatively, you can use the command :ref:`elem_save_custom_lib` in the schema window to create a preset from an existing element. Furthermore, you can use the :menuSelection:`Copy` command in the library window and the :ref:`Paste <elem_opers_paste>` command in the schema window. In this case it works as the usual paste operation, and the preset label is not used as a label prefix (see :ref:`app_settings_gen_labels_pasted`).

Use the :menuSelection:`Properties` button on the toolbar to change preset parameters. The change does not affect elements already created from the preset.

Use the :menuSelection:`Delete` button on the toolbar to remove the preset from the library and :doc:`catalog`. This does not affect elements already created from the preset.

.. note::
  The custom library is stored in the local user's profile, so it does not affect other users in the same system.

