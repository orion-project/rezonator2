.. index:: single: schema

******
光路
******

在 |rezonator| 中，被研究的光学系统称为*光路*。光路由若干光学部件、它们之间的间隔以及一些辅助对象组成。它们统称为 :doc:`元件 <elements>`。

光路保存在扩展名为 ``.REZ`` 的文件中。

你可以计算高斯光束在开放光学系统和稳定谐振腔中的传播。在当前版本中，|rezonator| 只能计算平面光学系统。“平面”是指这样一个系统：光轴的所有折转（反射、折射）都发生在同一平面上。这个平面称为 :ref:`切向平面 <working_planes>`。

开放光学系统在 |rezonator| 中称为*单程系统 (SP)*。例如望远镜、显微镜、扩束器等等。你必须设置一束输入光束，软件就能计算这束光如何通过光路的所有元件传播。

  .. image:: img/sketch_sp.png

谐振腔可以是*驻波谐振腔 (SW)* 或*环形谐振腔 (RR)*。借助本软件，你可以设想元件参数取什么值时谐振腔能进入稳定工作状态，以及谐振腔内自洽高斯光束具有哪些性质。

  .. image:: img/sketch_sw_rr.png

你可以随时通过 :doc:`往返类型对话框 <trip_type>` 更改光路的类型。项目窗口的状态栏会显示一个图标，标明所选的光路类型。该图标在下图中以 :GUIPart:`1` 标示。

每当你修改任何光路属性或元件参数时，|rezonator| 都会计算稳定性参数的值（针对 SW 和 RR）。当光路稳定性被破坏时，稳定性图标（在下图中以 :GUIPart:`2` 标示）会以红色高亮显示。不稳定谐振腔的光束性质计算不受支持。

  .. image:: img/status_bar_1.png
  
.. tocTree::
   :maxDepth: 1
   :caption: 目录：

   schema_window
   pumps_window
   params_window
   memo_window
   trip_type
   input_beam
   pump_mode
   wavelen
