# Configuration file for the Sphinx documentation builder.
# http://www.sphinx-doc.org/en/master/config

# -- Project information -----------------------------------------------------

project = 'rezonator2'
copyright = '2019, Nikolay Chunosov'
author = 'Nikolay Chunosov'

# The full version, including alpha/beta/rc tags
# TODO: change version automatically when run release script
release = '2.0.4-alpha4'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = [
    # http://www.sphinx-doc.org/en/master/usage/extensions/math.html
    'sphinx.ext.mathjax'
]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['img', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.
html_theme = 'alabaster'
