.. index:: single: RayVector (Python)
.. _py_ray_vector:

Class: RayVector
================

Represents a :ref:`ray vector <ray_vector>` (Y, V) in paraxial optics.

Constructor
-----------

Parameters:

- ``Y`` (float, optional) - Ray position (m)
- ``V`` (float, optional) - Ray angle (rad)

Example:

.. code-block:: python

    RayVector(Y=0, V=0)

Properties
----------

``Y`` (float, read/write)
    Ray position coordinate

``V`` (float, read/write)
    Ray angle coordinate

Indexing
--------

Vector components can be accessed by index:

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    Z.print(v[0])  # Y = 0.001
    Z.print(v[1])  # V = 0.01

Indexing also allows for unpacking a ray vector object into separate variables:

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    Y, V = v

Operations
----------

Addition
~~~~~~~~

.. code-block:: python

    v1 = Z.RayVector(0.001, 0.01)
    v2 = Z.RayVector(0.002, 0.02)
    v_sum = v1 + v2
    v1 += v2  # In-place addition

Matrix transformation
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    m = Z.Matrix(1, 0.1, 0, 1)
    v_out = m * v

Example
-------

.. code-block:: python

    import rezonator as Z

    # Create input ray
    ray_in = Z.RayVector(Y=0.001, V=0.01)

    # Transform through optical system
    lens = Z.Matrix(1, 0, -10, 1)
    ray_out = lens * ray_in

    Z.print('Input ray:', ray_in)
    Z.print('Output ray:', ray_out)

.. seeAlso::

  :doc:`py_api`, :doc:`py_matrix`, :doc:`py_matrix3`, :ref:`Example - Basic Ray Tracing <py_example_basic_ray_tracing>`
