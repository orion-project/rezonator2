.. _layout:

Layout
======

Schema layout is an automatically formed schematic representation of the schema in linear view, i.e., all bends and folds of the optical axis are ignored and elements are displayed in one line.

Here is the layout for the “z_crystal” example. It shows how the Z-shaped resonator is unwrapped into the linear view:

  .. image:: img/layout.png

Each element type has its own representation on the layout. Note that spherical mirrors are displayed as lenses if they are not placed at the schema's ends. If a beam falls angularly onto a mirror or a lens, the corresponding element is displayed inclined on the layout to indicate that the system is astigmatic.

  .. note:: Unlike the :doc:`schema_elems`, elements that are :ref:`disabled <elem_props_lock>` are not displayed on the layout.

Features
--------

Interactivity
~~~~~~~~~~~~~

The layout is interactive. You can click an element to select it. Hold the :menuSelection:`Ctrl` key to select several elements. The selection is synchronized with the selection in the :doc:`schema_elems`. When an element is selected on the layout, the context menu includes :ref:`some commands <elem_opers>` for elements. When you click on an empty space, the :menuSelection:`Copy Image` command appears in the context menu. It copies the layout content as an image into the clipboard.

Optional Labels
~~~~~~~~~~~~~~~

Element :ref:`labels <elem_props_label>` are shown above elements. To make a better view, you can hide some labels using the respective :ref:`element's option <elem_props_show_label>`.

  .. image:: img/layout_hide_labels.png

Narrow View
~~~~~~~~~~~

Element lengths are not displayed to scale on the layout. They all have fixed length. To make a better view, some element types, like :doc:`matrix/ElemEmptyRange` or :doc:`matrix/ElemTiltedInterface`, can have a narrower representation. This can be enabled by another :ref:`element's option <elem_props_draw_narrow>`.

  .. image:: img/layout_narrow_elem.png

.. seeAlso::

  - :doc:`schema_elems`
