# Images

## Directories

### `./_tmp`
Images used as shape sources, some obsolete images, or images moved from rezonator-1 but not used yet.

**TODO:** Should be eliminated. Move used images to respective dirs and delete unused ones.

### `./drawing`
Images that are used as tips/infos/descriptions in the application.

### `./elem`
Element drawings and icons.

### `./icon`
Images that are used as window header icons.

### `./misc`
This dir should contain images that are nor toolbar glyphs or window icons.

### `./tips`
Images displayed at start window in Tips section.

### `./toolbar`
Images mainly intended to be displayed in toolbars buttons and push buttons.

They can be `svg` images of any sizes (128x128, 512x512), but they should look fine even at small scales - 16x16, 24x24.

## SVG Optimization

[svgcleaner](https://github.com/RazrFalcon/svgcleaner) used for reducing the size of SVG-files that are built into the application resources. Almost all its settings are by default excepting coordinate precision and indentation.

```bash
svgcleaner \
    --coordinates-precision=2 \
    --properties-precision=2 \
    --transforms-precision=2 \
    --paths-coordinates-precision=2 \
    --indent=0 \
    in.svg \
    out.svg
```
