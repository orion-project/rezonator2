# How to load local libs on MacOS

This is an example of how to load `muparser` library located in the application bundle.

Libraries should be placed into `Frameworks` subdirectory of the application bundle.

Then we have to use `install_name_tool` to adjust resolving rules in application executable file:

```
install_name_tool -id \
    @executable_path/../Frameworks/libmuparser.2.dylib \
    bin/rezonator.app/Contents/Frameworks/libmuparser.2.dylib

install_name_tool -change \
    /usr/local/lib/libmuparser.2.dylib \
    @executable_path/../Frameworks/libmuparser.2.dylib \
    bin/rezonator.app/Contents/MacOS/rezonator
```