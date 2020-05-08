.. index:: single: pump mode

Pump Mode
=========

:menuselection:`Windows --> Pumps --> Pump --> Create (Ctrl+Ins)`

For SP schemas, you can specify parameters of the input beam in several different ways.

  .. image:: img/select_pump_mode.png


.. _pump_mode_waist:

Waist
-----

In this mode, you can specify a waist radius and a distance from the waist to the first element of the schema. 

The waist radius and the wavefront curvature radius of an equivalent Gaussian beam at the first element of the schema are calculated using these parameters and given M² value. Then the complex curvature radius of the equivalent Gaussian beam is calculated, and then it is affected by the schema matrices.

  .. image:: img/pump_waist.png


.. _pump_mode_front:

Front
-----

In this mode, you can specify a radius and a wavefront curvature of the input beam at the first element of the schema. 

The complex radius of curvature of the equivalent Gaussian beam is calculated using these parameters and given M² value. And then it is affected by the schema matrices. 

  .. image:: img/pump_front.png


.. _pump_mode_vector:

Ray Vector 
----------

In this mode, you can specify a radius and half of the spread angle of the input beam at the first element of the schema. 

No complex radius of curvature is calculated in this case and the M² parameter is not taken into account. The beam is considered in a geometrical approach, but not as gaussian beam. 

  .. image:: img/pump_ray.png


.. _pump_mode_sections:

Two Sections 
------------

This method is the same as the ‘Ray vector.’ However, instead of setting a divergence angle, you can specify the radii of two sections and a distance between them, and the divergence angle calculated. The beam is considered in a geometrical approach too.

  .. image:: img/pump_sects.png


.. _pump_mode_complex:

Complex or Inverse Complex 
--------------------------

In this mode you can specify real and imaginary parts of the complex radius of curvature of the input beam at the first element of the schema. 

If you set the M² parameter to a value different than 1, then it is supposed that real and imaginary parts correspond to the complex radius of curvature of a pseudo-gaussian beam. The complex parameter of the equivalent Gaussian beam is calculated using that parameter and given M² value. And then it is affected by the schema matrices.  

.. note::

  When you set up the input beam via its complex radius of curvature, you should bear in mind that the imaginary part of the inverse complex radius of curvature, and therefore the entire value of the complex radius of curvature, depends on the wavelength. If you change the working wavelength of the schema, then the same value of the complex radius of curvature becomes referring to another beam. 

|para|

.. seealso::

    :doc:`input_beam`, :doc:`pumps_window`