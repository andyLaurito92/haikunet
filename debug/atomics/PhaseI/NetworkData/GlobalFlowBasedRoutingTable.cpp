#include "GlobalFlowBasedRoutingTable.h"

std::multimap<GlobalFlowBasedRoutingTable::TableKey, GlobalFlowBasedRoutingTable::TableValue, GlobalFlowBasedRoutingTable::TableKeyComparator> GlobalFlowBasedRoutingTable::routingTable;
