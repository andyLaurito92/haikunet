module FlowConcreteBuilder
    def build_output_representation
        scilab_definition = build_scilab_definition
        cplusplus_definition = build_cplusplus_definition
        {'scilab'=>scilab_definition, 'cplusplus'=>cplusplus_definition}
    end

    def build_scilab_definition
        scilab_flows_definition = ''
        scilab_flows_definition += "\n"
        scilab_flows_definition += build_parameter_flow_distribution 'period', distribution_rate
        scilab_flows_definition += build_parameter_flow_distribution 'packetSize', distribution_size
        scilab_flows_definition
    end

    def build_parameter_flow_distribution(distribution_name, distribution_variable)
        scilab_flow_parameter_distribution = ''
        case distribution_variable
        when ConstantDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_CONSTANT;\n"            
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_value = #{distribution_variable.value};\n"
        when ExponentialDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_EXPONENTIAL;\n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_mu = #{distribution_variable.mu};\n"
        when NormalDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_NORMAL;\n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_mu = #{distribution_variable.mu}; \n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_var = #{distribution_variable.var}; \n"
        when ParetoDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_PARETO;\n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_val1 = #{distribution_variable.val_1}; \n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_val2 = #{distribution_variable.val_2}; \n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_mean = #{distribution_variable.mean}; \n"
        when SplitDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_SPLIT;\n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_shape = #{distribution_variable.shape}; \n"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_scale = #{distribution_variable.scale}; \n"
        when FelixDistribution
            scilab_flow_parameter_distribution = "flow#{@id}.#{distribution_name} =  DISTRIBUTION_FELIX;\n"
            scilab_flow_parameter_distribution += get_string_parameters_from (build_parameter_flow_distribution '', distribution_variable.period), "flow#{@id}.#{distribution_name}_period"
            scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_mode = #{distribution_variable.mode}; \n"
            if distribution_variable.mode == "FELIX_MODE_HIGH_THROUGHOUT"
                scilab_flow_parameter_distribution += get_string_parameters_from (build_parameter_flow_distribution '', distribution_variable.period), "flow#{@id}.#{distribution_name}_size_bytes"
                scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_buffer_bytes = #{distribution_variable.buffer_bytes}; \n"
                scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_timeout = #{distribution_variable.timeout}; \n"
                scilab_flow_parameter_distribution += "flow#{@id}.#{distribution_name}_out_size_bytes = #{distribution_variable.out_size_bytes}; \n"
            end
        else
            raise "Distribution provided has a class which was unexpected. Class was: #{distribution_variable.class}"
        end
        scilab_flow_parameter_distribution
    end

    def get_string_parameters_from(flow_definition_string, name_of_flow)
        output = ''
        flow_definition_lines = flow_definition_string.split "\n"
        distribution_line = flow_definition_lines.shift
        distribution_line.gsub( /\s+/, " " )
        distribution_line.gsub! 'flow.', ''
        distribution_line.gsub! '\n', ''
        distribution_line.gsub! '=', ''
        output += "#{name_of_flow} = #{distribution_line}"
        flow_definition_lines.each do |flow_definition_line|
            flow_definition_line.gsub( /\s+/, " " )
            flow_definition_line.gsub! 'flow.', ''
            flow_definition_line.gsub! '\n', ''
            flow_definition_line.gsub! '_', ''
            flow_definition_line.gsub! ';', ''
            key_and_value = flow_definition_line.split '='
            output += "#{name_of_flow}_#{key_and_value.first} = #{key_and_value.last}" 
        end
        output
    end

    def build_cplusplus_definition
        start_time_of_flow = 0
        # flow
        cplusplus_flows_definition = "\n"
        cplusplus_flows_definition += "\t ///// definition of flow #{@id} \n"
        cplusplus_flows_definition += "\t auto flow#{@id}PeriodDistribution = readDistributionParameter(\"flow#{@id}.period\"); \n"
        cplusplus_flows_definition += "\t auto flow#{@id}PacketSizeDistribution = readDistributionParameter(\"flow#{@id}.packetSize\"); \n"
        cplusplus_flows_definition += "\t auto flow#{@id} = std::make_shared<Flow>(\"#{@id}\", #{start_time_of_flow} /*startTime*/, #{priority} /*typeOfService*/, flow#{@id}PeriodDistribution, flow#{@id}PacketSizeDistribution); \n"
               
        #routes
        cplusplus_flows_definition += "\t // routes for flow #{@id} \n"
        paths.each_with_index do |path, index| #  add each route/path in the flow       
          cplusplus_flows_definition += "\t auto flow#{@id}_route#{index} = std::make_shared<Route>( std::deque<Route::Node>{ \n"
          path.links.each do |link|
              cplusplus_flows_definition += "\t\t\t {#{link.src_port}, \"#{link.src_element.id}.Routing\"}, \n"
          end
          last_link = path.links.last        
          cplusplus_flows_definition += "\t\t\t {#{last_link.dst_port}, \"#{last_link.dst_element.id}.Routing\"} \n"
          cplusplus_flows_definition += "\t });"
          cplusplus_flows_definition += "\t \n"
        end 
        
        
        # registrations
        cplusplus_flows_definition += "\t // register flow #{@id} with its routes\n"
        unique_sources = paths.map{ |path| path.links.first.src_element.id }.uniq{|id| id}  # only add unique sources
        unique_sources.each do |source|
          cplusplus_flows_definition += "\t FlowDefinitions::registerFlowSourceNode(flow#{@id}, \"#{source}.GeneratorApplication\");  \n"  
        end
        
        paths.each_with_index do |path, index| #  register each route/path in the flow  for each route
          cplusplus_flows_definition += "\t FlowDefinitions::registerFlowRoute(flow#{@id}, flow#{@id}_route#{index});  \n"
        end
                
        cplusplus_flows_definition += "\t \n"
        
        cplusplus_flows_definition
    end
end