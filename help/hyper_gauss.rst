Calculation of Hyper-Gaussian Beams
===================================

.. |MI| replace:: :math:`M^2`

Algorithm for calculation of hyper-Gaussian beams is applied to analyze single-pass systems (SP) when divergence parameter (|MI|) of the input beam set to a value greater than one.

Hyper-Gaussian beam
-------------------

Hyper-gaussian beam in reZonator is defined as a beam whose divergence is |MI| times greater than a Gaussian beam having the same waist radius. Therefore its Rayleigh distance is |MI| times less. See also [Beam Parameter Product](#TODO).

.. math:: V_{S,hyper} = M^2 V_{S,gauss}

.. math:: w_{0,hyper} = w_{0,gauss} = w_0

.. math:: z_{0,gauss} = \frac{w_0}{V_{S,gauss}}

.. math:: z_{0,hyper} = \frac{w_0}{V_{S,hyper}}

.. math:: z_{0,hyper} = \frac{w_0}{M^2 V_{S,gauss}} = \frac{z_{0,gauss}}{M^2}

Equivalent Gaussian beam
------------------------

To describe the propagation of hyper-Gaussian beam in free space, it is enough to replace the Rayleigh distance :math:`z_0` with :math:`z_0 / M^2` in formulas for :math:`w(z)` and :math:`R(z)`:

.. math:: w_{hyper}(z) = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_{0,hyper}} \bigg)^2 } = w_0 \sqrt{ 1 + \bigg( \frac{z M^2}{z_{0,gauss}} \bigg)^2 }

.. math:: R_{hyper}(z) = z \Bigg[ 1 + \bigg( \frac{z_{0,hyper}}{z} \bigg)^2 \Bigg] = z \Bigg[ 1 + \bigg( \frac{z_{0,gauss}}{z M^2} \bigg)^2 \Bigg]

It is equivalent to changing waist size from :math:`w_0` to :math:`w_0 / M`: 

.. math:: z_{0,hyper} = \frac{z_{0,gauss}}{M^2} = \frac{\pi w_0^2}{M^2 \lambda}

A beam having such a reduced waist size is called *Equivalent Gaussian Beam*:

.. math:: \frac{w_0^2}{M^2} = w_{0,equiv}^2

.. math:: z_{0,hyper} = \frac{\pi w_{0,equiv}^2}{\lambda} = z_{0,equiv}

Then we can express waist radius of hyper-Gaussian beam in terms of equivalent Gaussian beam:

.. math:: w_0 = M \times w_{0,equiv}

And substitute it into formulas for :math:`w(z)` and :math:`R(z)`:

.. math::
    w_{hyper}(z) = M \times w_{0,equiv} \sqrt{ 1 + \bigg( \frac{z}{z_{0,equiv}} \bigg)^2 }
    \longrightarrow \\
        w_{equiv}(z) = w_{0,equiv} \sqrt{ 1 + \bigg( \frac{z}{z_{0,equiv}} \bigg)^2 }

.. math::
    R_{hyper}(z) = z \Bigg[ 1 + \bigg( \frac{z_{0,equiv}}{z} \bigg)^2 \Bigg]
    \longrightarrow \\
        R_{equiv}(z) = z \Bigg[ 1 + \bigg( \frac{z_{0,equiv}}{z} \bigg)^2 \Bigg]

Since equivalent Gaussian beam has the same Rayleigh distance as hyper-Gaussian beam, its radius is :math:`M` times less than radius of original beam not only at the waist but anywhere. 

.. math:: w_{hyper}(z) = M \times w_{equiv}(z)

.. math:: R_{hyper}(z) = R_{equiv}(z)

Because equivalent Gaussian beam is a normal Gaussian beam, its complex parameter can be transformed by ray matrices as usual.

Algorithm of computing
----------------------

The program uses next algorithm for calculation of propagation of hyper-Gaussian beams: 

#. Equivalent size of the input beam is calculated. It is :math:`M` times less than specified in the dialog of input beam parameters. 

#. Propagation of equivalent Gaussian beam is calculated. 

#. Characteristics of the resulting hyper-Gaussian beam are calculated. Its radius is :math:`M` times greater than the radius of equivalent beam and radius of wavefront curvature is equal to that. 

.. image:: ./img/beam_pseudogauss.png
   :align: center

See also:
---------

[Gaussian Beam](#TODO)
