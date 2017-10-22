require_relative '../errors/debug_code_generator_error.rb'

module DebugCodeGenerator
    require_relative "#{File.dirname(File.realpath(__FILE__))}/../utils/dijkstra_algorithm.rb"

    #For debugging we use DEVS 
    K = 1000
    FLOW_DEFAULT_PRIORITY = 0
    FLOW_DEFAULT_GENERATION_PERIOD = ExponentialDistribution.new 1.0 / (100*K) # distribution period in seconds
    FLOW_DEFAULT_GENERATION_SIZE = NormalDistribution.new 4.0*K, 1.0*K  # (in bytes)

    def generate_output(file_name)
        @id_currently_flow = 0
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost                
                mac_value = host_value_of identifier, 'mac'
                ips_value = host_value_of identifier, 'ipAddresses'
                
                # We are going to define that a host exist in the topology if the mac is already defined.
                next if is_defined_in_topology mac_value, ips_value

                # If we are here, that means that the host does not exist in the topology. This means that
                # we need some extra info to define it. 
                vlan_value = host_value_of identifier, 'vlan'
                element_id_value = host_value_of identifier, 'elementId'
                port_value = host_value_of identifier, 'port'
                my_host = @initial_topology.add_host "#{mac_value}/#{vlan_value}", ips_value, mac_value

                switch = @initial_topology.get_element_by_id element_id_value
                raise_semantic_error "the switch #{element_id_value} is not defined in the initial topology!." unless switch

                @initial_topology.add_full_duplex_link  "Link#{mac_value.gsub ':', ''}_to_#{element_id_value.gsub ':', ''}", 
                                                my_host, 
                                                0,
                                                element_id_value,
                                                port_value.to_i
            when HaikunetFlow
                flow_params = get_flow_params identifier
                flow_params['src'].each do |src_mac|
                  flow_params['dst'].each do |dst_mac|
                    @initial_topology.add_flow  "Flow#{@id_currently_flow}", 
                                        FLOW_DEFAULT_PRIORITY, 
                                        [(find_path_between src_mac, dst_mac)], 
                                        FLOW_DEFAULT_GENERATION_PERIOD, 
                                        FLOW_DEFAULT_GENERATION_SIZE
                    @id_currently_flow += 1
                  end
                end
            when HaikunetIntent
                #Should read the action, and implement an automatic checker given the output
                #of Scilab
            end
        end
        topology_generator = Topologygenerator.new({
            "source" => "OBJECT",
            "directory_concrete_builders" => "#{File.dirname(File.realpath(__FILE__))}/../haikunet_builders/debug_builders",
            "output_directory" => "#{ENV['HOME']}/.haikunet/debug/#{file_name}",
            "uri_resource" => @initial_topology 
        })
        topology_generator.generate
    end

    def host_value_of(host_identifier, property)
        identifier = host_identifier.value.params.select{ |identifier| identifier.name == property }.first
        raise DebugCodeGeneratorError, "Property #{property} was not found in host identifier and is needed to create the simulation." if identifier.nil?
        identifier.value
    end

    def get_flow_params(flow_identifier)
        src = value_from 'src', flow_identifier.value.params
        src_mac = get_macs_from src
        dst = value_from 'dst', flow_identifier.value.params
        dst_mac = get_macs_from dst
        priority = value_from 'priority', flow_identifier.value.params

        {'src' => src_mac, 'dst' => dst_mac, 'priority' => priority}
    end

    def get_macs_from(resource)
      macs = []
      if resource.is_a? Array
        resource.each do |identifier|
          macs.push value_from 'mac', identifier.value.params
        end
      else
        macs.push value_from 'mac', resource.value.params
      end
      macs
    end

    def is_defined_in_topology(mac_value, ips_value)
        res = false
        host = (@initial_topology.elements_of_type Host).select { |host| host.mac == mac_value}.first 
        if host
            res = true
            warn "The host with mac #{mac_value} has changed the ip, new ips are #{ips_value}, while the old ones were #{host.ips}" unless ips_value.include? host.ips
        end
        res
    end

    def find_path_between(src_mac, dst_mac)
        links = @initial_topology.elements_of_type Link
        hosts = @initial_topology.elements_of_type Host

        graph = []
        links.each do |link|
            graph.push [link.src_element,link.dst_element,1]
        end

        src = hosts.find { |host| host.mac == src_mac }
        dst = hosts.find { |host| host.mac == dst_mac }

        solution = Dijkstra.new src, dst, graph
        path = solution.shortest_path

        #Now we have to find the link that connects both elements to add it to the path!
        my_path = Path.new src, dst
        path.each_with_index do |elem, index|
            break if path.size == (index + 1)
            my_path.add_link links.find{ |link| (link.src_element == elem) && (link.dst_element == path[index+1]) }
        end
        
        my_path
    end

    def value_from(value_name, array)
        array.select { |elem| elem.name == value_name }.first.value
    end
end