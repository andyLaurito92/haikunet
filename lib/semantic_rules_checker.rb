require "ipaddress"
require 'set'
require_relative 'utils/custom_file_utils.rb'
require_relative 'code_generators/code_generator.rb'

class SemanticRulesChecker
    def check(context, topology)
        validate_params context, topology

        @context = context
        @topology = topology

        static_properties
        
        dynamic_properties
    end

    def static_properties
        hosts_are_well_defined

        flows_are_well_defined

        values_defined_in_flows_are_defined_in_topology

        paths_for_flows_exists
    end

    def dynamic_properties
        # Right now, we are going to harcode in the simulator the topology model. TODO: Using the topology generated
        # from the simulator_code_generator.generate_code, create the model to be used as parameter to the simultaor
        # (The model creation in a normal process would be: 1)Design the topology using the simultaor; 
        # 2) Press the run buttom in PowerDEVS. This is what generates the model, that now is harcoded (meaning that
        # we are always using the same network).
        file_name = "simulation_debug_#{Time.now.getutc}".gsub! ' ', '_'

        simulator_code_generator = CodeGenerator.new
        simulator_code_generator.generate_code @context, 'DEBUG', file_name, @topology        
        
        #We copy the FlowDefinitions.cpp file to debug/atomics/PhaseI needed before compilation.   
        system "cp \"#{ENV['HOME']}/.haikunet/debug/#{file_name}/FlowDefinitions.cpp\" #{File.dirname(File.realpath(__FILE__))}/../debug/atomics/PhaseI"

        #Create the model executable
        system "#{File.dirname(File.realpath(__FILE__))}/../debug/bin/pdppt -pdif -x  \"#{ENV['HOME']}/.haikunet/debug/#{file_name}/topology.pdm\""

        #Run Scilab!
        scilab_process = fork do
          `#{File.dirname(File.realpath(__FILE__))}/../debug/bin/startScilab.sh`
        end

        sleep 40
        
        #Run the simulaton!
        system "cd #{File.dirname(File.realpath(__FILE__))}/../debug/output; ./RunNSimulations.sh -n 1 -f 0.1"

        Process.detach(scilab_process)
    end

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

        sources.each do |a_source|
            destinies.each do |a_destiny|
                raise_semantic_error "there is no path between source #{a_source.mac} and destiny #{a_destiny.mac}." unless (reacheables_elements_from a_source, Set.new).include? a_destiny.id
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
            #If the mac of the identifier is not found, we check again for the ip
            ips_value = host_value_of host_identifier, 'ipAddresses'
            hosts_in_topology.each do |host|
                return host if (host.ips == Array) && host.ips == ips_value
                return host if ips_value.include? host.ips
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

    def reacheables_elements_from(network_element, actual_set_of_reacheables)
        return unless network_element #There are some networks elements that can be nil
        return if actual_set_of_reacheables.include? network_element.id #This means we have already include the elements that we can reach from here!
        actual_set_of_reacheables.add network_element.id
        network_element.out_elements.each do |reacheable_network_element|
            reacheables_elements_from reacheable_network_element, actual_set_of_reacheables
        end
        actual_set_of_reacheables
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

        @topology.add_full_duplex_link  "Link#{mac_value.gsub ':', ''}_to_#{element_id_value.gsub ':', ''}", 
                                        my_host, 
                                        0,
                                        element_id_value,
                                        port_value.to_i

        my_host
    end

    def host_value_of(host_identifier, proerty)
        host_identifier.value.params.select{ |param| param.name == proerty }.first.value
    end

    def validate_params(context,topology)
        raise ArgumentError, "A valid context was expected as first argument of SemanticRulesChecker#check method, #{context} was recieved instead." unless (context.class == Hash) && (context.key? 'identifiers') && (context.key? 'intents')
        raise ArgumentError, "A topology was expected as second argument of SemanticRulesChecker#check method, #{topology} was recieved instead." unless topology.class == Topology
    end

    def raise_semantic_error(message)
        raise SemanticalError, "A semantic error was found in one of the flow definitions. The problem is that #{message}\n Please correct this error in order to run the program ;)."
    end
end