.. index:: single: python api

Python API
==========

|rezonator| provides a Python API that allows you to write custom calculation and plotting functions. These functions can access schema elements, perform calculations, and generate custom tables and plots. The API provides two main modules: :doc:`rezonator <py_global>` (aliased as ``Z`` for brevity) and :doc:`schema <py_schema>`.

.. tocTree::
   :maxDepth: 1
   :caption: API Modules and Classes:

   py_global
   py_schema
   py_element
   py_matrix
   py_matrix3
   py_ray_vector
   py_round_trip
   py_examples

Important Notes
---------------

Element Indexing
~~~~~~~~~~~~~~~~

Unlike regular Python lists, elements in schema are indexed starting from 1 to be consistent with how they are numbered in the :doc:`schema_elems`.

Measurement Units
~~~~~~~~~~~~~~~~~

All values are in SI units (meters, radians, etc.). Convert them as needed for display in text output. There is no need to convert for :doc:`table <custom_table>` and :doc:`plotting <custom_plot>` functions because both the :doc:`table <table_window>` and :doc:`plot <plot_window>` windows allow for choosing suitable measurement units to display values.

Temporary Changes
~~~~~~~~~~~~~~~~~

Use :ref:`Element.lock() <py_method_elem_lock>` and :ref:`Element.unlock() <py_method_elem_unlock>` methods when making temporary parameter :ref:`changes <py_method_elem_set_param>` (e.g., during :doc:`plot function <custom_plot>` calculation) to ensure proper restoration and also to preventing the UI from unnecessary updates.

Performance
~~~~~~~~~~~

:ref:`Round-trip matrix <round_trip>` calculation is performed before each beam parameter query (e.g., :ref:`RoundTrip.beam_radius() <py_method_rt_beam_radius>`). For multiple queries on the same parameter, store the result into a local variable rather than recalculating.

.. seeAlso::

  - :doc:`functions`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
