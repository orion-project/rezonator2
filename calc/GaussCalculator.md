# Gauss calculator tool formulas

*Gauss Calculator Tool* computes some free parameters of the Gaussian beam, when changing one of its parameters and fixing some others. As it is not possible to make all the parameters to be free, we introduce two fixing modes called *Lock waist* and *Lock front*.

- Lock waist - find such values of free parameters at which the beam waist $w_0$ stays constant when one of parameters changes.
- Lock front - find such values of free parameters at which the wavefront ROC $R$ and the beam radius $w$ at some axial distance $z$ stay constant when one of parameters changes.

Quadratic equations have two solutions and we have to define a way to point out which solution to take. So the tool has one additional parameter - zone - which can be *Near zone* or *Far zone*.

Test values for the tool are calculated via script `GaussCalculator.py`.


## Change waist $w_0$

### Lock waist
Calculate the beam radius $w$ and the wavefront ROC $R$ at given distance $z$.

$$ z_0 = \frac{\pi w_0^2}{M^2 \lambda}  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
For specified waist radius $w_0$, find such a distance $z$ and beam quality $M^2$ at which the wavefront ROC keeps its previously calculated value.

How to find: express $z_0$ from $w(z)$ and from $R(z)$, equate both expressions and solve against $z$.

$$ z = R \Bigg[ 1 - \bigg( \frac{w_0}{w} \bigg)^2 \Bigg] $$

$$ z_0 = \sqrt{ \frac{ z^2 w_0^2 }{ w^2 - w_0^2 } } $$

$$ z_0 = \sqrt{ z (R - z) }  $$

$$ M^2 = \frac{ \pi w_0^2 }{ \lambda z_0 }  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$


## Change Rayleigh length $z_0$

### Lock waist
With fixed waist radius $w_0$, find a value of the beam quality parameter $M^2$ which yields the specified $z_0$.

$$ M^2 = \frac{ \pi w_0^2 }{ \lambda z_0 }  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
For specified $z_0$, find such a distance $z$ and beam quality $M^2$ at which the wavefront ROC keeps its previously calculated value.

$$ z = \frac R 2 \pm \frac{\sqrt{ R^2 - 4 z_0^2 }}2  $$

$+$ sign is for the far zone, $-$ is for the near zone.

$$ w_0 = \sqrt{ \frac{w^2}{1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{ \pi w_0^2 }{ \lambda z_0 }  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$


## Change divergence angle $V_s$

### Lock waist
With fixed waist radius $w_0$, find such a value of beam quality parameter $M^2$ that yields to the specified angle.

$$ M^2 = \frac{ \pi w_0 V_s } \lambda $$

$$ z_0 = \frac{\pi w_0^2}{M^2 \lambda}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
For specified $V_s$, find such a distance $z$ and beam quality $M^2$ at which the wavefront ROC keeps its previously calculated value.

How to find $z$: substitute $z_0$ as $w_0/V_s$ into formulas for $w(z)$ and $R(z)$, express $w_0$ from both, equate them and solve against $z$.

$$ z = \frac{w^2}{R V_s^2} $$

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 


## Change axial distance $z$

### Lock waist

$$ z_0 = \frac{\pi w_0^2}{M^2 \lambda}  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$


## Change beam quality $M^2$

### Lock waist
The same as when the waist is changed.

$$ z_0 = \frac{\pi w_0^2}{M^2 \lambda}  $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
Taking a new value of $M^2$, find such a distance $z$ and waist radius $w_0$ preserving constant wavefront.

How to find $z$: express $w_0^2$ from formula for $R(z)$, substitute the expression into formula for $w(z)$ and solve it against $z$.

$$ z = \frac{w^4 \pi^2 R}{\big(M^2 \lambda R\big)^2 + w^4 \pi^2} $$

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$


## Change beam radius $w$

### Lock waist
Find beam quality parameter $M^2$ giving specified beam radius at the same axial distance $z$ and with the same waist radius $w_0$.

$$ z_0 = \frac{z w_0}{\sqrt{w^2 - w_0^2}} $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ R = z \Bigg[ 1 + \bigg( \frac{z_0}{z} \bigg)^2 \Bigg] $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
Find a waist radius $w_0$ and beam quality parameter $M^2$ giving specified beam radius at the same axial distance $z$.

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$


## Change wavefront ROC $R$

### Lock waist
Find beam quality parameter $M^2$ giving specified ROC at the same axial distance $z$ and with the same waist radius $w_0$.

$$ z_0 = \sqrt{z (R - z)}  $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ w = w_0 \sqrt{ 1 + \bigg( \frac{z}{z_0} \bigg)^2 } $$ 

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$

### Lock front
Find a waist radius $w_0$ and beam quality parameter $M^2$ giving specified ROC at the same axial distance $z$.

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

$$ q^{-1} = \frac 1 R + i \frac{\lambda}{\pi w^2} $$


## Change complex beam parameter $q$
Assigning either real or imaginary part of complex beam parameter, we define both the beam radius $w$ and the wavefront ROC $R$ at the same time.

$$ R = \frac{1}{Re\big(q^{-1}\big)} $$

$$ w = \sqrt{\frac{\lambda}{\pi Im\big(q^{-1}\big)}} $$

### Lock waist
Having fixed waist radius $w_0$, find axial distance $z$ and beam quality $M^2$ at which specified front is achieved.

$$ z = R \Bigg[ 1 - \bigg( \frac{w_0}{w} \bigg)^2 \Bigg] $$

$$ z_0 = \sqrt{z (R - z)}  $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$

### Lock front

$$ z_0 = \sqrt{z (R - z)}  $$

$$ w_0 = \frac{w}{\sqrt{ 1 + \big( z/z_0 \big)^2} } $$

$$ M^2 = \frac{\pi w_0^2}{z_0 \lambda} $$ 

$$ V_s = \frac{M^2 \lambda}{\pi w_0}  $$


## Change inverted complex beam parameter

Changing of real part is equivalent to changing of wavefront ROC $R$, and changing of imaginary part is equivalent to changing of beam radius $w$.


## Change wavelength $\lambda$

### Lock waist

The same formulas as when waist $w_0$ changes.

### Lock front

The same formulas as when beam radius $w$ or wavefront ROC $R$ changes.
