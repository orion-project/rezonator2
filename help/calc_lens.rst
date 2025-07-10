.. index:: single: lensmaker

Lensmaker
=========

.. |R1| replace:: `R`\ :sub:`1`
.. |R2| replace:: `R`\ :sub:`2`
.. |FF| replace:: `F`\ :sub:`F`
.. |FR| replace:: `F`\ :sub:`R`

:menuselection:`Tools --> Lensmaker`

The Lensmaker tool is a simplistic lens designer calculating focal ranges of a lens and drawing its shape. It uses standard `lensmaker's equation <https://en.wikipedia.org/wiki/Lens>`_ to calculate focal range of single lens in air.

    .. image:: img/calc_lens_draw.png

Lens parameters:
----------------

`D` - Lens diameter. It doesn't affect any calculations and only serves the drawing purposes.

|R1| - Radius of curvature of the left surface. Negative value means right-bulged surface, positive value means left-bulged surface. Use Inf for planar faces.

|R2| - Radius of curvature of the right surface. Negative value means right-bulged surface, positive value means left-bulged surface. Use Inf for planar faces.

`n` - Index of refraction of lens material. The tool only calculate lenses in air.

`T` - Lens thickness along optical axis. It's the distance between points `V` and `V'`.

.. note::
  Physically a planar surface have infinite radius of curvature. And one actually sometimes could see the Inf value in result fields. But here it is just a matter of convenience - it is easier to type 0 than Inf or something. And since a lens face can't really have zero ROC, the 0 value can be used for something practical.

.. note::
  Note that here the `standard sign convention <https://en.wikipedia.org/wiki/Radius_of_curvature_(optics)>`_ for optical surfaces is applied. While elements :doc:`matrix/ElemThickLens` and :doc:`matrix/ElemSphericalInterface` use an opposite sign convention that is for historical reason and will be changed to match the standard.

Calculated results:
-------------------

`F` - Effective focal length is the distance between focal point and corresponding principal point `F - H` and `F' - H'`. In general case there are two front and rear effective focal lengths, but when the medium before and after the lens is the same they are equal.

  .. tex:
    P = \cfrac{1}{F} = (n - 1)\bigg[\, 
        \cfrac{1}{R_1} - 
        \cfrac{1}{R_2} + 
        \cfrac{(n -1)T}{n R_1 R_2}
    \bigg]\,
  .. image:: img/calc_lens_f.png

`P` - Optical power in dioptres, see the above formula.

|FF| - Front focal range is the distance between the front focal point `F` and the left surface vertex point `V`.

  .. tex:
    F_F = -F \bigg(\, 1 + \cfrac{n - 1}{n R_2} T \bigg)\,
  .. image:: img/calc_lens_f1.png

|FR| - Rear focal range is the distance between the rear focal point `F'` and the right surface vertex point `V'`.

  .. tex:
    F_R = F \bigg(\, 1 - \cfrac{n - 1}{n R_1} T \bigg)\,
  .. image:: img/calc_lens_f2.png
