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
        create_scilab_folder_with_initial_files

        build_host_link_router_output

        build_flow_output
    end

    def create_scilab_folder_with_initial_files
        write_file "#{@output_directory}/Scilab/firstSimulation.sce",
                    first_simulation_sce

        write_file "#{@output_directory}/Scilab/eachSimulation.sce",
                    each_simulation_sce

        write_file "#{@output_directory}/Scilab/lastSimulation.sce",
                    last_simulation_sce

        write_file "#{@output_directory}/Scilab/metrics.sce",
                    metrics_sce

        write_file "#{@output_directory}/Scilab/functions.sce",
                    functions_sce

        write_file "#{@output_directory}/Scilab/model.scilabParams",
                    model_scilabparams                                
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

        write_file "#{@output_directory}/Scilab/#{OUTPUT_ROUTERS_SCILAB_FILE_NAME}",
                    router_scilab_params

        write_file "#{@output_directory}/Scilab/#{OUTPUT_LINKS_SCILAB_FILE_NAME}",
                    link_scilab_params

        write_file "#{@output_directory}/Scilab/#{OUTPUT_HOSTS_SCILAB_FILE_NAME}",
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

        write_file "#{@output_directory}/Scilab/#{OUTPUT_FLOWS_SCILAB_FILE_NAME}",
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
      
    
    def first_simulation_sce
      "printf('Starting Simulations ... \n');

      // This file is loaded into scilab for the first simulation
      // Perform here any scilab action for the very first simulation "
    end

    def each_simulation_sce
      "// This file is loaded into scilab each time a simulation finishes

      exec('#{@output_directory}/Scilab/functions.sce', 0);

      // Simulation Execution already update the ExperimentNumber for next run, so use the previous value
      expNumber = ExperimentNumber - 1;

      printf(\"--------------   Simulation %i Results -------------\n\", expNumber)
      // pre-calculate some results (WARNING: if there is an error in these calculations, nothing will be saved!!!)


      // fist of all, save results
      saveSimulationResults('ScilabLogs', expNumber);

      // print values in scilab
      printf(\"-----------------------------------\n\")"
    end

    def last_simulation_sce
      "printf(\"QT:Last Simulation Finished. \n\")

      // Perform post processing and plotting here

      // x initialisation
      x=[0:0.1:2*%pi]';
      //simple plot
      plot(sin(x))"
    end

    def metrics_sce
      "// Debug levels for each model (how much it is written to the pdevs_runX.log)
      debugLevel = LOG_LEVEL_NONE;
      "
    end

    def functions_sce
      "function [] = saveSimulationResults(saveDirectory, simu_index)
        dt = getdate();
        backupDirectory = sprintf('%s/%s_%i%.2i%.2i/', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
        logFileName = sprintf('%s_%i_%i%.2i%.2i_%2i%2i%2i', SimulationName, simu_index,  dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
        mkdir(backupDirectory);
        
        printf(\"Making a backup of all variables at \" + + backupDirectory + logFileName + \".sod   ...\n\")
        save(backupDirectory + logFileName + '.sod'); // saving all variables
        unix_g('cp pdevs*.log ' + backupDirectory ); // backup log
        unix_g('cp *.performance.log ' + backupDirectory ); // backup performance  
      endfunction

      function [ParameterValues] = completeParameterSpace(parameter_1, parameter_2)
        ParameterValues = [];
        [col, i_rows] = size(parameter_1);
        [col, j_rows] = size(parameter_2);
        for i = 1:i_rows
          for j = 1:j_rows
               ParameterValues(1,$+1) = parameter_1(i)
               ParameterValues(2,$) = parameter_2(j)
          end
        end
      endfunction

      function [ParameterValues] = complete3ParameterSpace(parameter_1, parameter_2, parameter_3)
        ParameterValues = [];
        [col, i_rows] = size(parameter_1);
        [col, j_rows] = size(parameter_2);
        [col, k_rows] = size(parameter_3);
        for i = 1:i_rows
          for j = 1:j_rows
            for k = 1:k_rows
                  ParameterValues(1,$+1) = parameter_1(i)
                  ParameterValues(2,$) = parameter_2(j)
                  ParameterValues(3,$) = parameter_3(k)
            end
          end
        end
      endfunction


      function [] = saveResults(fileName, directoryName)
        dt = getdate();
        
        // default parameters
        [out, inp] = argn(0);   
          if inp < 2 then, directoryName = sprintf('ScilabLogs/QT_%i%.2i%.2i/%.2d%.2d%.2d/', dt(1),dt(2),dt(6),dt(7), dt(8), dt(9)), end
          if inp < 1 then, fileName = sprintf('simuResults_%i%.2d%.2d_%.2d%.2d%.2d', dt(1),dt(2),dt(6), dt(7), dt(8), dt(9)), end
        
        mkdir(directoryName);
        
        printf(\"making a backup of all variables at \" + + directoryName + fileName + \".sod   ...\n\")
        unix_g('cp pdevs*.log ' + directoryName ); // backup log
        unix_g('cp *.performance.log ' + directoryName ); // backup performance log
        save(directoryName + fileName + '.sod'); // saving all variables 
      endfunction

      function [] = saveCurrentPlot()
        plotHandle = gcf();
        axes = gca();
        
        dt = getdate();
        saveDirectory = \"ScilabLogs\";
        backupDirectory = sprintf('%s/%s_%i%.2i%.2i', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
        mkdir(backupDirectory);
        plot_file_name = sprintf('%s/%s_%i%.2i%.2i_%2i%2i%2i', backupDirectory, axes.title.text, dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
        
        // save as SVG 
        xs2svg(plotHandle, plot_file_name);
      endfunction"
    end

    def model_scilabparams
      "exec('#{@output_directory}/Scilab/model.scilabParams', 0);

      // some extra constants
      TCP_MTU_bytes = 1500; // (can be set for Ethernet up to 9710B) searchnetworking.techtarget.com/definition/maximum-segment-size


      exec('#{@output_directory}/Scilab/metrics.sce', 0); // 
      exec('#{@output_directory}/Scilab/flows_definition.scilabParams', 0);
      exec('#{@output_directory}/Scilab/links_definition.scilabParams', 0);
      exec('#{@output_directory}/Scilab/routers_definition.scilabParams', 0);
      exec('#{@output_directory}/Scilab/hosts_definition.scilabParams', 0);

      // General
      StochasticSimu = 1;     // 1=YES (use STDEVS), 0=NO (use means as constant values).
      ReproducibleSimu = REPRODUCIBLE_SEED;   // When using STDEVS, 1=\"Uses always the same random numbers (Reproducible)\" 0=\"Uses unique random numbers (generates a new seed). Any other value will be used as seed\" 
      SimulationName = \"Haikunet_debug_simulation\";


      // Main parameters

      // For sweeping parameters: ./RunNSimulations.sh -n <sizeParameterValues> -f <finalTime>
      // Override these if necessary
      //ParameterNames = [\"variableName\"] ; // this must be changed in ScilabParameterSweeper::UpdateParameters
      //ParameterValues = [1 , 903.666 ]^-1;"
    end

end
