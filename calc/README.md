# Calculations

This directory contains various calculation scripts. These scripts provide a data to validation against in unit-tests (e.g. `Elem*` files for `../src/test/test_Elements.cpp`), or provide some calculations for functions verification.

## Prepare environment

The virtual environment shared between different project parts, e.g., `calc` and `help`, so it's better to make it on the project top level.

Linux, macOS:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Windows:

```bash
python -m venv .venv
.venv\Scripts\activate
```

There is no `python3` command on Windows so be sure Python 3 is in the `PATH`

## Install requirements

```bash
pip install -r requirements.txt
```

## Run notebook

While many scripts here are just Python code that should be run from terminal, there are several Jupiter notebooks. To view and edit them, run

```bash
jupyter notebook --port 9999
```

Port is optional, default is 8888.
