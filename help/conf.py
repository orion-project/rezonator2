# Configuration file for the Sphinx documentation builder.
# http://www.sphinx-doc.org/en/master/config

# -- Project information -----------------------------------------------------

project = 'rezonator'
copyright = '2006-2019, Nikolay Chunosov'
author = 'Nikolay Chunosov'

# The full version, including alpha/beta/rc tags
# TODO: change version automatically when run release script
release = '2.0.4-alpha4'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = [
]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['img', '.DS_Store']

primary_domain = None

rst_prolog = """
.. |para| replace:: \ \
"""

# -- Options for QtHelp output -----------------------------------------------

qthelp_namespace = 'org.orion-project.rezonator'

# -- Options for HTML output -------------------------------------------------

html_theme = 'agogo'
html_theme_options = {
  'headerbg': '#204a87',
  'footerbg': '#2e3436'
}

html_title = 'reZonator2 Manual'

# TODO: make header on transparent background
html_logo = '../img/rezonator_header.png'

html_favicon = '../img/icon/main_2.ico'
html_add_permalinks = ''
html_copy_source = False
