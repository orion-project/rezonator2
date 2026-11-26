Python API 示例
===================

本页提供了在 |rezonator| 中使用 :doc:`py_api` 的一些完整可运行示例。

.. #######################################################################

.. _py_example_basic_ray_tracing:

基本光线追迹
-----------------

使用 :doc:`custom_script` 运行该示例。

.. code-block:: python

  '''Matrix operations example'''
  import rezonator as Z

  def calculate():
    # Create matrices for optical elements
    lens = Z.Matrix(1, 0, -1/0.1, 1)    # Thin lens, f=100mm
    space = Z.Matrix(1, 0.05, 0, 1)     # Free space, d=50mm

    # Calculate system matrix
    system = space * lens * space

    Z.print('Lens matrix:', lens)
    Z.print('Space matrix:', space)
    Z.print('System matrix:', system)
    Z.print('System A:', system.A, 'D:', system.D)

    # Ray tracing
    ray_in = Z.RayVector(0.01, 0)  # Parallel ray at 10mm
    ray_out = system * ray_in

    Z.print('Input ray:', ray_in)
    Z.print('Output ray:', ray_out)

.. #######################################################################

.. _py_example_basic_schema_access:

基本光路访问
-------------------

本示例演示如何访问基本的光路信息、全局常量，并遍历各个元件。使用 :doc:`custom_script` 运行该示例。

.. code-block:: python

  '''Basic schema information'''
  import rezonator as Z
  import schema

  def calculate():
    Z.print('**** Global constants')
    Z.print(f'PI = {Z.PI}, C = {Z.C} m/s')

    Z.print('\n**** Schema parameters')
    wl = schema.wavelength()
    Z.print(f'Wavelength = {wl*1e9:.1f} nm')

    Z.print('\n**** Elements')
    for i in range(schema.elem_count()):
      elem = schema.elem(i+1)
      Z.print(f'{i+1}. {elem.label}')
      if elem.length is not None:
        Z.print(f' - L={elem.length*1e3:.2f} mm')
      Z.print('')

.. #######################################################################

.. _py_example_basic_elem_manipulation:

基本元件操作
--------------------------

使用 :doc:`custom_script` 运行该示例。

.. code-block:: python

  import rezonator as Z
  import schema

  def calculate():
    # Find an element having length
    elem = None
    for i in range(schema.elem_count()):
      el = schema.elem(i+1)
      if el.length is not None:
          elem = el
          break

    # Raise an error if not found
    if not elem:
      raise Exception('No suitable element found')

    # Get element information
    Z.print('Element:', elem.label)
    Z.print('  Length:', elem.length * 1000, 'mm')
    Z.print('  Optical path:', elem.optical_path * 1000, 'mm')
    Z.print('  Axial length:', elem.axial_length * 1000, 'mm')
    Z.print('  IOR:', elem.ior)

    # Modify parameter
    elem.set_param('L', elem.length + 0.001) # Increase by 1mm
    Z.print('Changed length:', elem.length * 1000, 'mm')

.. #######################################################################

.. _py_example_round_trip:

往返与光束属性
------------------------------

本示例展示如何计算往返矩阵、稳定性参数，以及切向和弧矢两个平面内的光束属性。使用 :doc:`custom_script` 运行该示例。

.. code-block:: python

  '''Round-trip matrix calculation'''
  from math import degrees as deg
  import rezonator as Z
  import schema

  def calculate():
    # Find an element having length
    elem = None
    for i in range(schema.elem_count()):
      el = schema.elem(i+1)
      if el.length is not None:
          elem = el
          break

    # Raise an error if not found
    if not elem:
      raise Exception('No suitable element found')

    Z.print('Reference:', elem.label)

    # Calculate in the middle of range
    elem.offset = elem.length / 2.0
    Z.print('Offset inside:', elem.offset * 1000, 'mm')

    rt_t = schema.round_trip(ref=elem, inside=True, plane='T')
    rt_s = schema.round_trip(ref=elem, inside=True, plane='S')

    Z.print('\nRound-trip matrices:')
    Z.print('Mt:', rt_t.matrix())
    Z.print('Ms:', rt_s.matrix())

    Z.print('\nStability (normal):')
    Z.print('Pt:', rt_t.stabil_nor)
    Z.print('Ps:', rt_s.stabil_nor)

    Z.print('\nStability (squared):')
    Z.print('Pt:', rt_t.stabil_sqr)
    Z.print('Ps:', rt_s.stabil_sqr)

    # Get beam properties
    Z.print('\nBeam properties:')
    (w_t, r_t, v_t) = rt_t.beam()
    (w_s, r_s, v_s) = rt_s.beam()
    Z.print('Beam size:', w_t*1e6, '×', w_s*1e6, 'mkm')
    Z.print('Wavefront ROC:', r_t*100, '×', r_s*100, 'cm')
    Z.print('Half angle:', deg(v_t), '×', deg(r_s), 'deg')

.. #######################################################################

.. _py_example_aper_ratio:

光阑损耗估算
-------------------------

一个使用自定义元件参数的示例：计算各元件处光阑尺寸与光束尺寸之比，以此作为可能发生光束截断风险的指标。使用 :doc:`custom_table` 运行该示例。

.. code-block:: python

  '''Aperture ratio calculation'''
  import rezonator as Z
  import schema

  def columns():
    '''Define table columns'''
    return [
      {
        'label': 'W',
        'title': 'Beam radius',
        'dim': Z.DIM_LINEAR
      },
      {
        'label': 'AR',
        'title': 'Aperture ratio',
        'dim': Z.DIM_NONE
      },
      {
        'label': 'AR2',
        'title': 'Square ratio',
        'dim': Z.DIM_NONE
      }
    ]

  def calculate(elem, pos, rt):
    '''Calculate aperture ratios for elements with aperture parameter'''

    def calc_ratio(aper):
      w = rt.beam_radius()
      # Treat aperture size as radius
      ar = aper / w
      ar2 = ar**2
      return {'W': w, 'AR': ar, 'AR2': ar2}

    # Check for aperture custom parameter
    aper = elem.param('A')
    if aper is not None and pos == POS_RIGHT:
      return calc_ratio(aper)

    # For elements with length, check left and right apertures
    if elem.length is not None:
      if pos == POS_LEFT:
        aper = elem.param('A1')
        if aper is not None:
          return calc_ratio(aper)
      if pos == POS_RIGHT:
        aper = elem.param('A2')
        if aper is not None:
          return calc_ratio(aper)

    return None

.. #######################################################################

.. _py_example_custom_caustic:

自定义焦散函数
-----------------------

本示例演示如何创建一个自定义绘图函数：计算沿某个元件的光束半径，并将其显示为焦散图。使用 :doc:`custom_plot` 运行该示例。

.. code-block:: python

  '''Caustic plot - beam radius vs position'''
  import rezonator as Z
  import schema

  # Find an element with length for scanning
  elem = None
  for i in range(schema.elem_count()):
    el = schema.elem(i+1)
    if el.length is not None:
      elem = el
      break

  if not elem:
    raise Exception('No suitable element found')

  def figure():
    '''Configure plot properties'''
    return {
      'x_dim': Z.DIM_LINEAR,
      'x_title': f'Position in {elem.label}',
      'y_dim': Z.DIM_LINEAR,
      'y_title': 'Beam radius',
    }

  def calculate():
    '''Calculate beam radius along element'''
    Z.print('Calculating caustic at', elem.label)

    (xt, yt) = calc_plane('T', 100)
    (xs, ys) = calc_plane('S', 100)

    return [
      {'label': 'T', 'x': xt, 'y': yt},
      {'label': 'S', 'x': xs, 'y': ys}
    ]

  def calc_plane(plane, point_count):
    '''Calculate beam radius for one plane'''
    rt = schema.round_trip(ref=elem, inside=True, plane=plane)
    step = elem.axial_length / (point_count - 1)

    x, y = [], []
    for i in range(point_count):
      offset = step * i
      elem.offset = offset
      x.append(offset)
      y.append(rt.beam_radius())

    return (x, y)

更多示例
-------------

参见内置示例，以更深入地了解如何使用 Python API 进行自定义计算。使用 :menuSelection:`File --> Open Example` 菜单命令，并在过滤字段中输入 "python"。

  .. image:: img/open_example_py.png

.. seeAlso::

  - :doc:`py_api`
  - :doc:`custom_funcs`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
