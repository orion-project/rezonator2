# Configuration file for the Sphinx documentation builder.
# http://www.sphinx-doc.org/en/master/config

import os
import datetime

def get_file_text(file_name):
    with open(file_name, 'r') as f:
        return f.read()

def set_file_text(file_name, text):
    with open(file_name, 'w') as f:
        f.write(text)

# -- Project information -----------------------------------------------------

project = 'rezonator'
author = 'Nikolay Chunosov'
copyright = '2006-{}, {}'.format(datetime.datetime.now().year, author)
release = get_file_text(os.path.join('..', 'release', 'version.txt'))


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = [
]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['img', '.DS_Store', '.venv']

primary_domain = None

rst_prolog = """
.. |para| replace:: \ \

.. role:: elem_param

.. role:: param

.. role:: math_var

.. role:: app_name

.. role:: app_name_z

.. |rezonator| replace:: :app_name:`re`\ :app_name_z:`Z`\ :app_name:`onator`

.. default-role:: math_var

.. role:: guipart
"""

# .. |rezonator| replace:: :app_name:`rezonator`
# .. |rezonator| replace:: :app_name:`re`\ :app_name_z:`Z`\ :app_name:`onator`

# -- Options for QtHelp output -----------------------------------------------

qthelp_namespace = 'org.orion-project.rezonator'

# The alabaster theme looks cool in Assistant,
# but plus/minus buttons on the toolbar stop working - they only change the size of headers, not of all the text
# and font configuration page in settings dialog becomes useless - it can't change the font of the theme
# qthelp_theme = 'alabaster'
# qthelp_theme_options = {
#   'show_powered_by': False
# }

# -- Options for HTML output -------------------------------------------------

html_theme = 'agogo'
html_theme_options = {
  'headerbg': '#204a87',
  'footerbg': '#2e3436'
}

html_title = 'reZonator2 Manual'

html_logo = '../img/rezonator_header.png'

html_favicon = '../img/icon/main_2.ico'
html_permalinks = False
html_copy_source = False

html_show_copyright = False

html_static_path = ['styles']

#------------------------------------------------

about_text = get_file_text('about.html')
about_text = about_text.replace('VERSION', release)
about_text = about_text.replace('COPYRIGHT', copyright)
target_dir = os.path.join('..', 'out', 'help')
if not os.path.isdir(target_dir): os.makedirs(target_dir, exist_ok=True)
set_file_text(os.path.join(target_dir, 'about.html'), about_text)
