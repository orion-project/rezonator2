# Documentation

reZonator user manual is built using [Sphinx Documentation Builder](http://www.sphinx-doc.org).

## Install

Linux:
```bash
sudo python3 -m pip install sphinx
```

MacOC:
```bash
python3 -m pip install sphinx
```

Windows:
```bash
python -m pip install sphinx
```
Python for Windows doesn't provide alias `python3` so be sure Python 3 is in `PATH`

## Build

Linux/MacOS:

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

As the documentation contains a notable amount of physical formulas, they should be inserted as pictures. Qt Assistant uses `QTextDocument` to display help pages, and it has fewer abilities comparing to real browsers. Also, Assistant can't run JavaScript code. So pictures for formulas is the only possibility to display them. The page `render_formula.html` used to render TeX syntax into SVG using well known [MaxJax](https://github.com/mathjax/MathJax) library. PNG images then saved using print-screen operation, no automation for this process yet.

There also a number of ODF files that are LibreOffice Math formulas. They should be considered as deprecated and gradually replaced. Reasons are Math uses its own syntax instead of conventional TeX, and its rendered formulas do not have such nice appearance as those rendered by MathJax.
