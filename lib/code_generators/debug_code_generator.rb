module DebugCodeGenerator
    #For debugging we use DEVS 
    FLOW_DEFAULT_PRIORITY = 0
    FLOW_DEFAULT_GENERATION_PERIOD = ExponentialDistribution.new 1.0 / (100*K) # distribution period in seconds
    FLOW_DEFAULT_GENERATION_SIZE = NormalDistribution.new 4.0*K, 1.0*K  # (in bytes)

    def generate_output(file_name)
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost
                mac_value = host_value_of host_identifier, 'mac'
                ips_value = host_value_of host_identifier, 'ipAddresses'
                vlan_value = host_value_of host_identifier, 'vlan'
                element_id_value = host_value_of host_identifier, 'elementId'
                port_value = host_value_of host_identifier, 'port'
                my_host = @topology.add_host "#{mac_value}/#{vlan_value}", ips_value, mac_value

                switch = @topology.get_element_by_id element_id_value
                raise_semantic_error "the switch #{element_id_value} is not defined in the initial topology!." unless switch

                @topology.add_full_duplex_link  "Link#{mac_value.gsub ':', ''}_to_#{element_id_value.gsub ':', ''}", 
                                                my_host, 
                                                0,
                                                element_id_value,
                                                port_value.to_i
            when HaikunetFlow
                flow_params = get_flow_params identifier
                flow_params['src'].each do |src_mac|
                  flow_params['dst'].each do |dst_mac|
                    @topology.add_flow  "Flow#{index}_#{eLinkIndex}", 
                                        FLOW_DEFAULT_PRIORITY, 
                                        [find_path_between src_mac, dst_mac], 
                                        FLOW_DEFAULT_GENERATION_PERIOD, 
                                        FLOW_DEFAULT_GENERATION_SIZE
                  end
                end
            when HaikunetIntent
                #Should read the action, and implement an automatic checker given the output
                #of Scilab
            end
        end
        topology_generator = Topologygenerator.new({
            "source" => "ONOS",
            "directory_concrete_builders" => "#{File.dirname(File.realpath(__FILE__))}/haikunet_builders/debug_builders",
            "output_directory" => "#{ENV['HOME']}/.haikunet/debug/#{@file_name}",
            "uri_resource" => uri_initial_topology 
        })
        topology_generator.generate
    end

    def find_path_between(src_mac, dst_mac)

    end

    def value_from(value_name, array)
        array.select { |elem| elem.name == value_name }.first.value
    end
end