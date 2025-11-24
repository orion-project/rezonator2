.. _custom_params:
.. index:: single: custom parameters

Custom Parameters
=================

:menuselection:`Element --> Properties`

Custom parameters allow you to extend elements with additional user-defined parameters beyond their standard built-in parameters. This is useful when you need to track or calculate additional properties specific to your optical system. Custom parameters can be created, edited, and removed. They behave like regular element parameters and can be:

- Linked to :ref:`global parameters <params_window>`
- Referenced in :doc:`custom functions <custom_funcs>`
- Adjusted using the :doc:`Adjustment tool <adjust>`

The custom parameters functionality is accessed through a menu button located in the upper-right  corner of the :guilabel:`Parameters` tab in the :doc:`elem_props` dialog. The menu provides commands for managing custom parameters and parameter presets.

  .. image:: img/custom_params.png

.. --------------------------------------------------------------------------

Manage Custom Parameters
------------------------

.. _custom_params_create:

Create New Parameter
~~~~~~~~~~~~~~~~~~~~

:menuselection:`Parameters menu --> Add Parameter...`

When creating a new custom parameter, you have to specify the parameter properties:

- **Label** - Short identifier used in formulas and tables (e.g., ``Pth``, ``alpha``)
- **Name** - Full descriptive name (e.g., "Threshold power", "Brewster angle") displayed in the Element Properties dialog when the parameter is selected
- **Description** - Optional detailed description of the parameter displayed in the Element Properties dialog when the parameter is selected
- **Dimension** - Physical dimension (e.g., Linear, Angular)

Optionally check :guilabel:`Save as preset` to make this parameter available for reuse (:ref:`see below <custom_params_presets>`).

  .. image:: img/custom_params_create.png

The new parameter is added to the element's parameters list with an initial value of zero in the most recently used unit for its dimension. You can then:

- Set its value directly
- Use an :ref:`expression <elem_props_params>`
- :ref:`Link it <elem_props_link>` to a global parameter

.. --------------------------------------------------------------------------

.. _custom_params_edit:

Edit Parameter
~~~~~~~~~~~~~~

:menuselection:`Parameters menu --> Edit Parameter...`

The :guilabel:`Edit Parameter` command is only available when a custom parameter is selected. Built-in element parameters cannot be edited this way.

When you change a custom parameter's dimension, the system automatically converts the current value using SI unit conversion using the most recently used unit for the new dimension. For example, if a parameter currently has a value of ``10 cm`` (Linear dimension) and you change it to Angular dimension, the value converts as ``10 cm → 0.1 m → 0.1 rad → 5.73 deg``. The parameter is reassigned with the converted value in the recent unit for angular dimensions.

  .. warning::
    When changing dimensions, it is your responsibility to ensure the parameter is still used appropriately. If a parameter is linked to a global parameter with a different dimension, the values will be converted using SI units, which may produce unexpected results.

.. --------------------------------------------------------------------------

.. _custom_params_remove:

Remove Parameter
~~~~~~~~~~~~~~~~

:menuselection:`Parameters menu --> Remove Parameter...`

The :guilabel:`Remove Parameter` command is only available when a custom parameter is selected. Built-in element parameters cannot be removed.

When you remove a custom parameter:

- Any :ref:`parameter link <elem_props_link>` to a global parameter is automatically removed
- The parameter is deleted from the element
- Schema events are triggered to notify other parts of the application

.. --------------------------------------------------------------------------

.. _custom_params_presets:

Parameter Presets
-----------------

Parameter presets allow you to quickly add commonly-used custom parameters without having to specify all properties each time. The system provides both built-in presets and supports custom presets. Custom presets are saved globally in the user's system profile and available for all elements and schemas.

When :ref:`creating a new parameter <custom_params_create>`, check the :guilabel:`Save as preset` option to save it as a reusable preset. The parameter's configuration (label, name, description, and dimension) will be available in the presets menu for future use.

To create a parameter from a preset, select a preset from the list (built-in or custom presets are shown). The parameter is instantly added with the preset's configuration.

Each preset in the menu has a dropdown arrow that provides additional options allowing you to modify the preset configuration (custom presets only) or to remove the preset. Built-in presets cannot be edited or removed, but you can hide them by removing them from the configuration and later restore them using :guilabel:`Restore Default Presets`.

Restoring Default Presets
~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuselection:`Parameters menu --> Restore Default Presets...`

Use this command to clean the presets collection and restore all built-in presets.

  .. warning::
    Restoring default presets cannot be undone. All your custom presets *will be lost*.

.. --------------------------------------------------------------------------

Best Practices
--------------

- **Meaningful Names** - Choose descriptive aliases and labels that clearly indicate the parameter's purpose
- **Consistent Dimensions** - When linking custom parameters to global parameters, ensure dimensions match or understand the SI conversion that will occur
- **Use Presets** - Save frequently-used custom parameters as presets to maintain consistency across your schemas
- **Document Parameters** - Use the Name and Description fields to provide clear documentation, especially for complex calculations or physical meanings
- **Avoid Over-complicating** - Create custom parameters only when necessary; excessive parameters can make schemas harder to understand

.. --------------------------------------------------------------------------

.. seeAlso::

  - :doc:`elem_props`
  - :doc:`custom_funcs`
  - :doc:`Global Parameters <params_window>`
