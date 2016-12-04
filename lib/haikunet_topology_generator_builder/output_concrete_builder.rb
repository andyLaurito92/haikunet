module OutputConcreteBuilder
    OUTPUT_RUBY_FILE_NAME = 'initial_network_topology.rb'

    def initialize_concrete_builder(topology_provider, directory_concrete_builders, output_directory)
        @topology_provider = topology_provider
        @directory_concrete_builders = directory_concrete_builders # make's sense?
        @output_directory = output_directory
    end

    def build_output_content
        graph_elements = topology_provider.get_topology
        
        graph_elements = graph_elements.select { |elem| [Host,Link,Router].include? elem.class }

        ruby_network_topology = "module NetworkTopology \n"

        get_topology_function_def ="    def get_topology \n "
        get_topology_function_def +="       @topology = Topology.new \n "
        get_topology_function_def +="       hosts = [] \n "
        get_topology_function_def +="       routers = [] \n" 
        get_topology_function_def +="       links = [] \n" 
        get_topology_function_def +="\n"

        get_topology_function_def += add_elements_of_type graph_elements, Host
        get_topology_function_def += add_elements_of_type graph_elements, Router
        get_topology_function_def += add_elements_of_type graph_elements, Link

        get_topology_function_def +="      @topology.topology_elements \n "
        get_topology_function_def +="   end \n"

        get_path_definition_function_def = "    def get_path_between(source, destination) \n"
        get_path_definition_function_def += "        raise NotImplementedError, \"NetworkTopology: This method is not implemented (¿does it ever get called?)\" \n"
        get_path_definition_function_def += "   end \n"
                                                
        ruby_network_topology += get_topology_function_def
        ruby_network_topology += "\n"
        ruby_network_topology += get_path_definition_function_def
        ruby_network_topology += 'end'


        write_file "#{@output_directory}/#{OUTPUT_RUBY_FILE_NAME}",
                    ruby_network_topology
    end

    def add_elements_of_type(graph_elements, type)
        elements_to_add = graph_elements.select { |elem| elem.is_a? type }

        elements_def = ''

        elements_to_add.each do |element|
            elements_def += "      #{element.transform_to_output_representation @directory_concrete_builders} \n"
        end

        elements_def = replace_ids_host_router_for_index graph_elements, elements_def if type == Link

        elements_def += "\n"
        elements_def
    end

    def replace_ids_host_router_for_index(graph_elements, links_definition)
        # Links have a dst and a src. These elements are in one of either hosts or routers array, 
        # however the index is unkown for the Link!, Instead of serializing the src/dst, the link
        # prints it's element id. We now have to find this id (which is a string), and replace it by the 
        # corresponding element of the array.
        hosts = graph_elements.select { |elem| elem.is_a? Host }
        routers = graph_elements.select { |elem| elem.is_a? Router }

        hosts.each_with_index do |host,index|
            links_definition.gsub! host.id, "hosts[#{index}]"
        end

        routers.each_with_index do |router,index|
            links_definition.gsub! router.id, "routers[#{index}]"
        end

        links_definition
    end
end
