.. index:: single: lua

Lua Primer
==========

|rezonator| uses `Lua <http://www.lua.org>`_ internally to calculate user formulas.


    
    
Supported Mathematical Functions
--------------------------------

Lua provides the set of `mathematical functions <https://www.lua.org/manual/5.3/manual.html#6.7>`_ . One have to call them using the library name, e.g. ``math.sin(math.pi / 4)`` 

``sin`` --- returns the sine of the angle in radians.

``sinh`` --- returns the hyperbolic sine of the argument.

``asin`` --- returns the arcsine of the argument as an angle in radians.

``cos`` --- returns the cosine of the angle in radians.

``cosh`` --- returns the hyperbolic cosine of the argument.

``acos`` --- returns the arccosine of the argument as an angle in radians.

``tan`` --- returns the tangent of the angle in radians.

``tanh`` --- returns the hyperbolic tangent of the argument.

``atan`` --- returns the arctangent of the argument as an angle in radians.

``cot`` --- returns the cotangent of the angle in radians.

``coth`` --- returns the hyperbolic cotangent of the argument.

``acot`` --- returns the arccotangent of the argument as an angle in radians.

``sec`` --- returns the secant of the angle in radians.

``sech`` --- returns the hyperbolic secant of the argument.

``csc`` --- returns the cosecant of the angle in radians.

``csch`` --- returns the hyperbolic cosecant of the argument.

``abs`` --- returns the absolute value of the argument.

``floor`` --- returns the largest integer that is not greater than the argument. For example, ``floor(41.2) = 41``.

``ceil`` --- returns the smallest integer that is not less than the argument. For example, ``ceil(41.2) = 42``.

``exp`` --- returns the value of `e` to the power of the argument, where `e` is the base of natural logarithms.

``ln`` --- returns the natural logarithm of the argument. Natural logarithm uses base `e`.

``lg`` --- returns the logarithm of the argument in base 10.

``sqrt`` --- returns the square root of the argument.

``deg2rad`` --- converts the angle from degrees to radians.

``rad2deg`` --- converts the angle from radians to degrees.
