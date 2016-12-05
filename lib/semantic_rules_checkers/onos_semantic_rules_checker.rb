module OnosSemanticRulesChecker
    def hosts_are_well_defined

    end

    def flows_are_well_defined

    end

    def values_defined_in_flows_are_defined_in_topology
        hosts_in_topology = @topology.elements_of_type Host
        flows_identifiers_defined = @context['identifiers'].select{ |identifier| identifier.value.is_a? HaikunetFlow }

        #If a HaikunetHost is used as parameter of the flow, then semantically is well 
        #constructed, since if the Host is not defined, it will be constructed first. 

        identifiers_in_flow_parameters = flows_parameters_of_type flows_identifiers_defined, String 

        #We just want an array of string, since this is the only case in which a mac or an ip could 
        #be used for destiny or source. If the array is of HaikunetHostIdentifiers, the we keep going
        identifiers_in_flow_parameters += (flows_parameters_of_type flows_identifiers_defined, Array)
        identifiers_in_flow_parameters = identifiers_in_flow_parameters.flatten
        identifiers_in_flow_parameters = identifiers_in_flow_parameters.select { |parameter| parameter.is_a? String }

        identifiers_in_flow_parameters.each do |identifierValue|
            if IPAddress.valid? identifierValue
                check_if_property_exists_in_topology hosts_in_topology, identifierValue, lambda{|host, value| return true if host.ips.include? value }, "the ip #{identifierValue} is not defined in the initial topology provided."
            elsif  MacAddress.valid? identifierValue
                check_if_property_exists_in_topology hosts_in_topology, identifierValue, lambda{|host, value| return host.mac. == value}, "the mac #{identifierValue} is not defined in the initial topology provided."
            end
        end
    end

    def paths_for_flows_exists
        flows_identifiers_defined = @context['identifiers'].select{ |identifier| identifier.value.is_a? HaikunetFlow }

        flows_identifiers_defined.each do |flow_identifier|
            check_if_path_exist_in flow_identifier
        end
    end

    def flows_parameters_of_type(flows, resource)
        resources = []
        flows.each do |flow_identifier|
            haikunet_flow_identifier = flow_identifier.value
            haikunet_flow_identifier.params.each do |identifier|
                resources.push identifier.value if identifier.value.is_a? resource #Because the first identifier is the tag
            end
        end
        resources
    end

    def check_if_property_exists_in_topology(hosts_in_topology, value, condition_to_check, error_message)
        hosts_in_topology.each do |host|
            return if condition_to_check.call host, value
        end
        raise_semantic_error error_message
    end

    def check_if_path_exist_in(flow)
        sources_identifier = flow.value.params.select{ |param| param.name == 'src' }.first.value
        destinies_identifier = flow.value.params.select{ |param| param.name == 'dst' }.first.value

        sources_identifier = [sources_identifier] unless sources_identifier.is_a? Array
        destinies_identifier = [destinies_identifier] unless destinies_identifier.is_a? Array

        obtaining_host_representation_from_identifiers = lambda{ |hosts_identifiers| 
            hosts_representations = []
            hosts_identifiers.each do |host_identifier|
                host_representation = obtain_host_topology_representation_of host_identifier
                hosts_representations.push host_representation if host_representation != "NONE"
            end
            return hosts_representations
        }
        
        sources = obtaining_host_representation_from_identifiers.call sources_identifier
        destinies = obtaining_host_representation_from_identifiers.call destinies_identifier

        sources.each do |one_src|
            destinies.each do |one_dst|
                check_if_path_exist_between one_src, one_dst
            end
        end        
    end

    def obtain_host_topology_representation_of(host_identifier)
        hosts_in_topology = @topology.elements_of_type Host
        #A host_identifier can be either a property of the host in the topology, or a HaikunetHostIdentifier or an Array!. 
        #In this second case, it can happens that the host is not defined in the initial topology!.
        if host_identifier.is_a? HaikunetIdentifier
            mac_value = host_value_of host_identifier, 'mac'
            hosts_in_topology.each do |host|
                return host if host.mac == mac_value
            end
            #If we are here, then it means that the host is not defined in the actual topology!. This means that
            #we have first to define it in the topology whith it's link.
            return define_host_identifier_in_topology host_identifier
        #Is a property of a host     
        elsif host_identifier.is_a? String 
            if IPAddress.valid? host_identifier
                return hosts_in_topology.select{ |host| host.ips.include? host_identifier }.first
            elsif MacAddress.valid? host_identifier
                return hosts_in_topology.select{ |host| host.mac == host_identifier }.first
            end
        else
            raise_semantic_error "the identifier #{host_identifier} is not a valid identifier for a host."
        end
        return "NONE"
    end

    def check_if_path_exist_between(source, destiny)
        #The solution could be Disjoint-set !
        @topology.topology_elements
    end

    def define_host_identifier_in_topology(host_identifier)
        mac_value = host_value_of host_identifier, 'mac'
        ips_value = host_value_of host_identifier, 'ipAddresses'
        vlan_value = host_value_of host_identifier, 'vlan'
        element_id_value = host_value_of host_identifier, 'elementId'
        port_value = host_value_of host_identifier, 'port'
        my_host = @topology.add_host "#{mac_value}/#{vlan_value}", ips_value, mac_value

        switch = @topology.get_element_by_id element_id_value
        raise_semantic_error "the switch #{element_id_value} is not defined in the initial topology!." unless switch

        @topology.add_link  "Link#{mac_value.gsub ':', ''}_to_#{element_id_value.gsub ':', ''}", 
                            my_host, 
                            0,
                            element_id_value,
                            port_value.to_i

        @topology.add_link  "Link##{element_id_value.gsub ':', ''}_to_{mac_value.gsub ':', ''}", 
                            element_id_value, 
                            port_value.to_i,
                            my_host,
                            0                            
        my_host
    end

    def host_value_of(host_identifier, proerty)
        host_identifier.value.params.select{ |param| param.name == proerty }.first.value
    end
end