# Documentation

reZonator user manual is built using [Sphinx Documentation Builder](http://www.sphinx-doc.org).

## Prepare environment

The virtual environment shared between different project parts, e.g., `calc` and `help`, so it's better to make it on the project top level.

Linux, macOS:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Windows (there is no `python3` command on Windows so be sure Python 3 is in the PATH):

```bash
python -m venv .venv
.venv\Scripts\activate
```

## Install requirements

For building help we only need

```bash
python -m pip install sphinx
```

## Build

Linux, macOS:

```bash
./make.sh
```

Windows:

```bash
make.bat
```

Target documentation is in `../out` directory.

## Notes

### Formulas

As the documentation contains a notable amount of physical formulas, they should be inserted as pictures. Qt Assistant uses `QTextDocument` to display help pages, and it has fewer abilities comparing to real browsers. Also, Assistant can't run JavaScript code. So pictures for formulas is the only possibility to display them. The page `render_formula.html` used to render TeX syntax into SVG using well known [MathJax](https://github.com/mathjax/MathJax) library. PNG images then saved using print-screen operation, no automation for this process yet.

There also a number of ODF files that are LibreOffice Math formulas. They should be considered as deprecated and gradually replaced. Reasons are Math uses its own syntax instead of conventional TeX, and its rendered formulas do not have such nice appearance as those rendered by MathJax.
