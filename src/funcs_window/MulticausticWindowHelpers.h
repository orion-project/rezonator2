#ifndef MULTICAUSTIC_WINDOW_HELPERS_H
#define MULTICAUSTIC_WINDOW_HELPERS_H

#include "../core/Units.h"

#include <QList>

class CausticFunction;
class QCPCursor;

namespace MulticausticWindowHelpers {

void showRoundTrip(Z::Unit unitX, QCPCursor* cursor, const QList<CausticFunction*>& funcs, const QString& baseTitle);

} // namespace MulticausticWindowHelpers

#endif // MULTICAUSTIC_WINDOW_HELPERS_H
