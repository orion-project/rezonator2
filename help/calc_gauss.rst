Gaussian Beam Calculator
========================

.. |z_0| replace:: `z`\ :sub:`0`
.. |w_0| replace:: `w`\ :sub:`0`
.. |V_s| replace:: `V`\ :sub:`s`
.. |M^2| replace:: `M`\ :sup:`2`

:menuselection:`Tools --> Gaussian Beam Calculator`

.. Text and formulas are in the algorithm description `./calc_gauss.html`, keep these docs in sync

The tool *Gauss Beam Calculator* computes some free parameters of the Gaussian beam when changing one of its parameters and fixing some others. As it is not possible to make all the parameters to be free, we introduce two fixing modes called *Lock waist* and *Lock front*.

- *Lock waist* - find such values of free parameters at which the beam waist |w_0| stays constant when one of the parameters changes.
- *Lock front* - find such values of free parameters at which the wavefront ROC `R` and the beam radius `w` at some axial distance `z` stay constant when one of the parameters changes.

Quadratic equations have two solutions and we have to define a way to point out which solution to take. So the tool has one additional parameter - zone - which can be *Near zone* or *Far zone*.

The first two buttons on the toolbar control the locking mode, and the next two - what solution is to be displayed.

  .. image:: img/wnd_gauss_calc_w.png

Beam Parameters
---------------

These parameters are assigned and computed: 

* `λ` -  light wavelength; it is not calculated from other parameters, only assigned
* |w_0| - waist radius 
* |z_0| - Rayleigh range (half-confocal parameter)
* |V_s| - beam divergence angle in the far-field zone 
* |M^2| - beam quality factor (M-square parameter)
* `z` - distance from the waist to a point where the wavefront is calculated or assigned
* `w` - beam radius on distance `z` from the waist
* `R` - radius of curvature of the wavefront on distance `z` from the waist
* `re(q)`, `im(q)` - real and imaginary parts of complex beam parameter on distance `z` from the wais
* `re(1/q)`, `im(1/q)` - real and imaginary parts of inverted complex beam parameter on distance `z` from the wais
  
  .. image:: img/wnd_gauss_calc_r.png


Calculation Algorithm
---------------------


Change waist |w_0|
^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

Calculate the beam radius `w` and the wavefront ROC `R` at given distance `z`:

  .. image:: img/f_calc_gauss_w0_1.png
 
Lock front
~~~~~~~~~~

For specified waist radius |w_0|, find such a distance `z` and beam quality |M^2| at which the wavefront ROC keeps its previously calculated value.

How to find: express |z_0| from `w(z)` and from `R(z)`, equate both expressions and solve against `z`.

  .. image:: img/f_calc_gauss_w0_2.png

  
Change Rayleigh length |z_0|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

With fixed waist radius |w_0|, find a value of the beam quality parameter |M^2| which yields the specified |z_0|.

  .. image:: img/f_calc_gauss_z0_1.png

Lock front
~~~~~~~~~~

For specified |z_0|, find such a distance `z` and beam quality |M^2| at which the wavefront ROC keeps its previously calculated value. `+` sign is for the far zone, `-` is for the near zone.

  .. image:: img/f_calc_gauss_z0_2.png
 

Change divergence angle |V_s|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

With fixed waist radius |w_0|, find such a value of beam quality parameter |M^2| that yields to the specified angle.

  .. image:: img/f_calc_gauss_Vs_1.png

Lock front
~~~~~~~~~~

For specified |V_s|, find such a distance `z` and beam quality |M^2| at which the wavefront ROC keeps its previously calculated value.

How to find `z`: substitute |z_0| as |w_0|/|V_s| into formulas for `w(z)` and `R(z)`, express |w_0| from both, equate them and solve against `z`.

  .. image:: img/f_calc_gauss_Vs_2.png


Change axial distance `z`
^^^^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

  .. image:: img/f_calc_gauss_z_1.png

Lock front
~~~~~~~~~~

  .. image:: img/f_calc_gauss_z_2.png


Change beam quality |M^2|
^^^^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

The same as when the waist is changed.

  .. image:: img/f_calc_gauss_MI_1.png

Lock front
~~~~~~~~~~

Taking a new value of |M^2|, find such a distance `z` and waist radius |w_0| preserving constant wavefront.

How to find `z`: express |w_0|:sup:`2` from formula for `R(z)`, substitute the expression into formula for `w(z)` and solve it against `z`.

  .. image:: img/f_calc_gauss_MI_2.png


Change beam radius `w`
^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

Find beam quality parameter |M^2| giving specified beam radius at the same axial distance `z` and with the same waist radius |w_0|.

  .. image:: img/f_calc_gauss_w_1.png

Lock front
~~~~~~~~~~

Find a waist radius |w_0| and beam quality parameter |M^2| giving specified beam radius at the same axial distance `z`.

  .. image:: img/f_calc_gauss_w_2.png


Change wavefront ROC `R`
^^^^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

Find beam quality parameter |M^2| giving specified ROC at the same axial distance `z` and with the same waist radius |w_0|.

  .. image:: img/f_calc_gauss_R_1.png

Lock front
~~~~~~~~~~

Find a waist radius |w_0| and beam quality parameter |M^2| giving specified ROC at the same axial distance `z`.

  .. image:: img/f_calc_gauss_R_2.png


Change complex beam parameter `q`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Assigning either real or imaginary part of complex beam parameter, we define both the beam radius `w` and the wavefront ROC `R` at the same time.

  .. image:: img/f_calc_gauss_q_0.png

Lock waist
~~~~~~~~~~

Having fixed waist radius |w_0|, find axial distance `z` and beam quality |M^2| at which specified front is achieved.

  .. image:: img/f_calc_gauss_q_1.png

Lock front
~~~~~~~~~~

  .. image:: img/f_calc_gauss_q_2.png


Change inverted complex beam parameter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Changing of real part is equivalent to changing of wavefront ROC `R`, and changing of imaginary part is equivalent to changing of beam radius `w`.


Change wavelength `λ`
^^^^^^^^^^^^^^^^^^^^^

Lock waist
~~~~~~~~~~

The same formulas as when waist |w_0| changes.

Lock front
~~~~~~~~~~

The same formulas as when beam radius `w` or wavefront ROC `R` changes.
