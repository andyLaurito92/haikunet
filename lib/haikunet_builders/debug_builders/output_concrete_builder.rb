module OutputConcreteBuilder
    NUMBER_OF_PDM_MODELS_IN_STRUCTURE = 3
  
    OUTPUT_PDM_FILE_NAME = 'topology.pdm'
    OUTPUT_CPP_FILE_NAME = 'FlowDefinitions.cpp'
    OUTPUT_FLOWS_SCILAB_FILE_NAME = 'flows_definition.scilabParams'
    OUTPUT_ROUTERS_SCILAB_FILE_NAME = 'routers_definition.scilabParams'
    OUTPUT_LINKS_SCILAB_FILE_NAME = 'links_definition.scilabParams'
    OUTPUT_HOSTS_SCILAB_FILE_NAME = 'hosts_definition.scilabParams'
    
    def initialize_concrete_builder(topology_provider, directory_concrete_builders, output_directory)
        @topology_provider = topology_provider
        @directory_concrete_builders = directory_concrete_builders
        @output_directory = output_directory
    end

    def build_output_content
        build_host_link_router_output

        build_flow_output
    end

    def build_host_link_router_output
        graph_elements = topology_provider.get_topology
        pdm_topology = pmd_initial_struture 
        
        router_scilab_params = ''
        link_scilab_params = ''
        host_scilab_params = ''

        elements_which_are_not_flows = graph_elements.select{ |elem| [Host,Router,Link].include? elem.class }

        elements_which_are_not_flows.each do |element|
            output_representation = element.transform_to_output_representation @directory_concrete_builders
            pdm_topology += output_representation['pdm']
            router_scilab_params += output_representation['scilab'] if element.is_a? Router
            link_scilab_params += output_representation['scilab'] if element.is_a? Link
            host_scilab_params += output_representation['scilab'] if element.is_a? Host
        end
        
        pdm_topology += create_lines_between_graph_elements graph_elements

        pdm_topology += pdm_final_structure

        write_file "#{@output_directory}/#{OUTPUT_PDM_FILE_NAME}",
                    pdm_topology

        write_file "#{@output_directory}/#{OUTPUT_ROUTERS_SCILAB_FILE_NAME}",
                    router_scilab_params

        write_file "#{@output_directory}/#{OUTPUT_LINKS_SCILAB_FILE_NAME}",
                    link_scilab_params

        write_file "#{@output_directory}/#{OUTPUT_HOSTS_SCILAB_FILE_NAME}",
                    host_scilab_params
    end

    def create_lines_between_graph_elements(graph_elements)
        links = graph_elements.select { |node| node.is_a? Link }
        lines = ''
        links.each do |link|
            src_pdm_pos = get_pdm_position link.src_element, graph_elements
            dst_pdm_pos = get_pdm_position link.dst_element, graph_elements
            link_pdm_pos = get_pdm_position link, graph_elements
            lines += link.create_pdm_line_between_src_and_dst src_pdm_pos, dst_pdm_pos, link_pdm_pos
        end
        lines
    end

    def get_pdm_position(network_elem, graph_elements)
        #Here links DONT have a modeled representation, so we just have to count routers and hosts
        return -1 if network_elem.is_a? Link
        
        network_elem_index = graph_elements.index { |elem| elem.id == network_elem.id }
        my_pdm_position = 0
        (0..network_elem_index).each do |i|
            my_pdm_position += 1 unless graph_elements[i].is_a? Link
        end
        NUMBER_OF_PDM_MODELS_IN_STRUCTURE + my_pdm_position
    end    

    def build_flow_output
        graph_elements = topology_provider.get_topology 

        flows = graph_elements.select { |elem| elem.is_a? Flow }        

        scilab_flows_definition = ''
        cplusplus_flows_defined = ''
        flows.each do |flow|
            flow_output = flow.transform_to_output_representation @directory_concrete_builders
            scilab_flows_definition += flow_output['scilab']
            cplusplus_flows_defined += flow_output['cplusplus']
        end

        write_file "#{@output_directory}/#{OUTPUT_FLOWS_SCILAB_FILE_NAME}",
                    scilab_flows_definition

        write_file  "#{@output_directory}/#{OUTPUT_CPP_FILE_NAME}",
                   (build_cplusplus_output cplusplus_flows_defined)
    end

    def build_cplusplus_output(cplusplus_flows_defined)
        cplusplus_flows_definition = "#include \"FlowDefinitions.h\" \n"
        cplusplus_flows_definition += "\n"
        cplusplus_flows_definition += "std::multimap<std::string, std::shared_ptr<Flow>> FlowDefinitions::Flows; \n"
        cplusplus_flows_definition += "\n"
        cplusplus_flows_definition += "void FlowDefinitions::defineFlows(){ \n"
        cplusplus_flows_definition += "\n"
        cplusplus_flows_definition += cplusplus_flows_defined
        cplusplus_flows_definition += '}'
        cplusplus_flows_definition
    end
    
    def pmd_initial_struture
      "Coupled
          {
              Type = Root
              Name = MyTopology
              Ports = 0; 0
              Description = Testing the creation of a topology by getting the info from the controller
              Graphic
                  {
                      Position = 0; 0
                      Dimension = 600; 600
                      Direction = Right
                      Color = 15
                      Icon = 
                      Window = 5000; 5000; 5000; 5000
                  }
              Parameters
                  {
                  }
              System
                  {
                  Atomic
                      {
                      Name = LoadScilabParams
                      Ports = 1 ; 0
                      Path = sinks/RunScilabJob.h
                      Description = If Scilab is configured as backed in the cmd line options, this model runs Scilab commands at Init, Exit and when receive events.
                      Graphic
                          {
                          Position = -13845 ; -14220 
                          Dimension = 540 ; 540
                          Direction = Right
                          Color = 15
                          Icon = %sinks%scilab.ico
                          }
                      Parameters
                          {
                          Run at Init = Str; exec('#{File.absolute_path(@output_directory)}/Scilab/model.scilabParams', 0) ; Scilab Job at Init
                          Run at External = Str;  ; Scilab Job when receive event
                          Run at Exit = Str;  ; Scilab Job at Exit
                          }
                      }
                  Atomic
                      {
                      Name = ExperimenetTracker
                      Ports = 0 ; 0
                      Path = sinks/SimulationExperimentTracker.h
                      Description = Allows to use multiple simulation runs setting new parameter values in each run. It configures Scilab variables according to the current simunation number. This model should run with 1st priority!!
                      Graphic
                          {
                          Position = -11220 ; -14220
                          Dimension = 540 ; 540
                          Direction = Right
                          Color = 15
                          Icon = %realtime%lcd.svg
                          }
                      Parameters
                          {
                          ScilabSimulationSetID = Str; SimulationName ; indicates the simulation set ID
                          ScilabSimulationCounterVariableName = Str; ExperimentNumber ; Name of the Scilab variable that indicates the simulation number.
                          ScilabParametersVariableName = Str; ParameterValues ; Name of the Scilab variable that contains the parameter values for each simulation
                          ScilabParametersValuesVariableName = Str; ParameterNames ; Name of the Scilab variable that contains the parameter names for each simulation
                          }
                      }
                  Atomic
                      {
                      Name = UpdateScilabParams
                      Ports = 1 ; 0
                      Path = sinks/RunScilabJob.h
                      Description = If Scilab is configured as backed in the cmd line options, this model runs Scilab commands at Init, Exit and when receive events.
                      Graphic
                          {
                          Position = -8670 ; -14220
                          Dimension = 540 ; 540
                          Direction = Right
                          Color = 15
                          Icon = %sinks%scilab.ico
                          }
                      Parameters
                          {
                          Run at Init = Str;  ; Scilab Job at Init
                          Run at External = Str;  ; Scilab Job when receive event
                          Run at Exit = Str;  ; Scilab Job at Exit
                          }
                      }
              "
    end  
    
    def pdm_final_structure
      "Atomic
                      {
                      Name = FinalizationCommands
                      Ports = 0 ; 0
                      Path = sinks/multipleSimulationCommands.h
                      Description = Executes Scilab commands when using multiple simulation runs (at the end of each simulation, and at the end of ALL simulations).\\nThis model should run with LAST priority
                      Graphic
                          {
                          Position = -6270 ; -14220
                          Dimension = 540 ; 540
                          Direction = Right
                          Color = 15
                          Icon = %datanetworks%scilab.bmp
                          }
                      Parameters
                          {
                          initSimulationCommandName = Str; #{File.absolute_path(@output_directory)}/Scilab/firstSimulation.sce ; 
                          eachSimulationCommandName = Str; #{File.absolute_path(@output_directory)}/Scilab/eachSimulation.sce ; 
                          lastSimulationCommandName = Str; #{File.absolute_path(@output_directory)}/Scilab/lastSimulation.sce ; 
                          }
                      }
                  }
          }"
    end
      
    
end
