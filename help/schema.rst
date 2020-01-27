.. index:: single: schema

******
Schema
******

An optical system under investigation is called *schema* in terms of |rezonator|. The schema consists of several optical components, ranges between them, and some helper objects. They all are called :doc:`elements <elements>`.

Schemas are saved in files with the extension ``.REZ``.

You can calculate propagation of the Gaussian beam through an open optical system and a stable resonator. In the current version, |rezonator| can calculate only planar optical systems. ‘Planar’ means such a system where all breaks of the optical axis (reflections, refractions) do occur on the same plane. This plane is called the :ref:`tangential plane <working_planes>`. 

An open optical system is called a *single-pass system (SP)* in |rezonator|. For example, it is a telescope, microscope, beam expander, and so on. You have to set-up an input beam, and the software can compute how this beam propagates through all elements of the schema. 

  .. image:: img/sketch_sp.png

A resonator can be a *standing wave resonator (SW)* or *ring resonator (RR)*. With the help of the software, you can conceive which value of element parameters brings resonator to a stable operation regime and which properties the self-consistent Gaussian beam has inside the resonator. 

  .. image:: img/sketch_sw_rr.png

You can change the type of schema through the :doc:`Trip-type dialog <trip_type>` at any time. The status bar of the project window displays an icon denoting selected schema type. The icon pointed as :guipart:`1` on the screenshot below.

Each time you modify any schema properties or elements' parameters, |rezonator| computes a value of stability parameter (for SW and RR). When schema stability gets broken, then the stability icon (shown as :guipart:`2` on the screenshot below) gets highlighted in red color. Calculation of beam properties for unstable resonators is not supported.

  .. image:: img/status_bar_1.png
  
.. toctree::
   :maxdepth: 1
   :caption: Table of Contents:

   trip_type
   input_beam
   pumps_window
   pump_mode
 