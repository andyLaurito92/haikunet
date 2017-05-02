#include "FlowDefinitions.h" 

std::multimap<std::string, std::shared_ptr<Flow>> FlowDefinitions::Flows; 

void FlowDefinitions::defineFlows(){ 


	 ///// definition of flow Flow0 
	 auto flowFlow0PeriodDistribution = readDistributionParameter("flowFlow0.period"); 
	 auto flowFlow0PacketSizeDistribution = readDistributionParameter("flowFlow0.packetSize"); 
	 auto flowFlow0 = std::make_shared<Flow>("Flow0", 0 /*startTime*/, 0 /*typeOfService*/, flowFlow0PeriodDistribution, flowFlow0PacketSizeDistribution); 
	 // routes for flow Flow0 
	 auto flowFlow0_route0 = std::make_shared<Route>( std::deque<Route::Node>{ 
			 {0, "Host1.Routing"}, 
			 {2, "Router1.Routing"}, 
			 {0, "Host2.Routing"} 
	 });	 
	 // register flow Flow0 with its routes
	 FlowDefinitions::registerFlowSourceNode(flowFlow0, "Host1.GeneratorApplication");  
	 FlowDefinitions::registerFlowRoute(flowFlow0, flowFlow0_route0);  
	 
}
