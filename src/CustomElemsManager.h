#ifndef CUSTOM_ELEMS_MANAGER_H
#define CUSTOM_ELEMS_MANAGER_H

class Element;
class Schema;

namespace CustomElemsManager {

/// Loads custom elements library.
/// The caller is responsible for deletion of the returned schema.
Schema* load();

/// Saves a copy of the given element into the custom elements library.
/// The `schema` parameter is the element's owner,
/// it's used for checking for elements' relations.
void saveAsCustom(Schema *schema, Element* elem);

}

#endif // CUSTOM_ELEMS_MANAGER_H
