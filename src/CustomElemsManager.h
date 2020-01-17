#ifndef CUSTOM_ELEMS_MANAGER_H
#define CUSTOM_ELEMS_MANAGER_H

class Element;
class Schema;

namespace CustomElemsManager {

Schema* load();
void saveAsCustom(Schema *schema, Element* elem);

}

#endif // CUSTOM_ELEMS_MANAGER_H
