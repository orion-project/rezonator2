#include "HelpSystem.h"

#include "helpers/OriDialogs.h"

namespace Z {
namespace Help {

void show(const QString& topic)
{
    Ori::Dlg::info(topic); // TODO: show help
}

}}
