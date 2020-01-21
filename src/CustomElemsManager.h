#ifndef CUSTOM_ELEMS_MANAGER_H
#define CUSTOM_ELEMS_MANAGER_H

#include "core/OriResult.h"

class Element;
class Schema;

namespace CustomElemsManager {

/// Returns a path to the custom elements library;
QString libraryFile();

/// Loads the custom elements library.
/// The caller is responsible for deletion of the returned schema.
Ori::Result<Schema*> loadLibrary();

/// Saves the custom elements library.
QString saveLibrary(Schema* library);

/// Saves a copy of the given element into the custom elements library.
/// The `schema` parameter is the element's owner,
/// it's used for checking for elements' relations.
QString saveToLibrary(Element* elem);

}

#endif // CUSTOM_ELEMS_MANAGER_H
