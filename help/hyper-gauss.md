# Calculation of Pseudo-Gaussian Beams 

Algorithm for calculation of pseudo-gaussian beams is applied to analyze single-pass systems (SP) when divergence parameter (M²) of input beam is set to value greater than one. 

## Pseudo-Gaussian beam

Pseudo-gaussian beam in reZonator is defined as a beam whose divergence is M² times greater than a Gaussian beam having the same waist radius. Therefore its Rayleigh distance is M² times less. See also [beam parameter product](#TODO). 

$$ w_{0,pseudo} = w_{0,gauss} = w_0 $$

$$ V_{S,pseudo} = M^2 V_{S,gauss} $$

$$ z_{0,gauss} = \frac{w_0}{V_{S,gauss}} $$

$$  z_{0,pseudo} = \frac{w_0}{M^2 V_{S,gauss}} = \frac{z_{0,gauss}}{M^2} $$

## Equivalent Gaussian beam

To describe propagation of pseudo-gaussian beam in free space, it is enough to replace the Rayleigh distance from $z0$ to $z0 / M²$ in formulas for $w(z)$ and $R(z)$ (see formula for [radius of Gaussian beam](#TODO)). It is equivalent to changing of waist size from $w0$ to $w0 / M²$. A beam having such reduced waist size is called *Equivalent Gaussian Beam*. 

$$ z_0 \rightarrow \frac{z_0}{M^2} $$

$$ \frac{\pi w_0^2}{\lambda} \rightarrow \frac{\pi w_0^2}{M^2 \lambda} $$

$$ w_0 \rightarrow \frac{w_0}{M^2} $$

$$ w_{0,equiv} = \frac{w_{0,pseudo}}{M} $$

Since equivalent gaussian beam has the same Rayleigh distance as pseudo-gaussian beam, its radius is $M$ times less than radius of original beam not only at the waist but anywhere. 

$$ z_{0,equiv} = z_{0,pseudo} = z_0  $$

$$ w_{equiv}(z) = w_{0,equiv} \sqrt{1 + (z/z_0)^2} $$

$$ w_{pseudo}(z) = w_{0,pseudo} \sqrt{1 + (z/z_0)^2} $$

$$ \Downarrow $$

$$ w_{pseudo}(z) = M \times w_{equiv}(z)  $$

## Algorithm of computing
The program uses next algorithm for calculation of propagation of pseudo-gaussian beams: 

* Equivalent size of source of input emission is calculated. It is $M$ times less than specified in dialog of parameters of input beam. 

* Propagation of equivalent Gaussian beam is calculated. 

* Characteristics of resulting pseudo-gaussian beam are calculated. Its radius is $M$ times greater than radius of equivalent beam and radius of wavefront curvature is equal to that. 

![](./img/beam_pseudogauss.png)

## See also:
[Gaussian Beam](#TODO)

