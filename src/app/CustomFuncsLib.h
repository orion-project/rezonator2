#ifndef CUSTOM_FUNCS_LIB_H
#define CUSTOM_FUNCS_LIB_H

#include "core/OriResult.h"

namespace CustomFuncsLib {

/// Creates a file in the custom function library
/// or replaces code in the given of found full path.
/// Shows a confirmation dialog for replacement
/// if a function with the same name already exists.
/// Returns a full path to created or updated file.
QString put(const QString &code, const QString &targetPath = {});

/// Save given custom code to given full file path.
/// Shows an error dialog and returns false if failed.
bool saveCode(const QString &code, const QString &filePath);

/// Load custom function code from file by full path
Ori::Result<QString> loadCode(const QString &filePath);

/// Get all absolute paths of all files in the library directory
QStringList getAllFiles();

/// Find existing file with the given user-readable name.
/// Returns absolute file path if found, empty string otherwise
QString findExistingFile(const QString &displayName);

/// Extract user-readable name from Python code docstring.
/// Returns the first non-empty line of the docstring or empty string if not found.
QString getDisplayName(const QString &code);

/// Delete a custom function file.
/// Shows an error dialog and returns false if failed.
bool remove(const QString &filePath);

}

#endif // CUSTOM_FUNCS_LIB_H
