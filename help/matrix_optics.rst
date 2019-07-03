.. index:: single: matrix optics

Calculation of Beam Parameters in Resonators
============================================

The description is given according to the book "Lasers" by A.E. Siegman, see chapter 21 "Generalized paraxial resonator theory". 

For the analysis, we need to select a reference plane to calculate :doc:`the round trip matrix<round_trip_matrix>` against. In general, an element chosen as a parameter of a particular function is the reference plane.

Having the round-trip matrix we can write an expression for transformation of :ref:`complex parameter<gauss_complex_param>`:

    .. image:: img/gauss_q_abcd.png

Because we consider a stable resonator, the output parameter is the same as the input one, so the beam is self-consistent, and it reproduces itself after full round-trip through the resonator:

    .. image:: img/abcd_q_in_out.png
    
|para|

    .. image:: img/abcd_q.png

Then we can find out the complex parameter as a solution to the quadratic equation:

    .. image:: img/abcd_q_quadratic.png


But for convenience, it is better to rewrite this equation meaning the inverted complex parameter:

    .. image:: img/abcd_q_quadratic_1.png


using the relationship

    .. image:: img/abcd_ad_bc.png

Self-conformed Complex Radius of Curvature
------------------------------------------

The solution to this equation gives values of the complex parameter of self-consistent Hermite-gaussian mode in the resonator:

    .. image:: img/abcd_solution.png

or

    .. image:: img/abcd_solution_1.png


One of the solutions gives unreal situation when the beam's energy increases with radial distance.

Knowing the expression of the complex parameter through beam characteristics

    .. image:: img/gauss_q.png

we can express the properties of the self-consistent beam in terms of round-trip matrix elements.

Radius of Wavefront Curvature
-----------------------------

Calculation of the radius of wavefront curvature of the self-consistent beam from components of round-trip matrix:

    .. image:: img/abcd_curvature.png
   
   
Beam Radius
-----------

Calculation of the self-consistent beam radius from components of round-trip matrix:

    .. image:: img/abcd_radius_1.png

|para|

    .. image:: img/abcd_radius_2.png
    
|para|

    .. image:: img/abcd_radius_3.png
    
.. seealso::

    :doc:`gauss`
