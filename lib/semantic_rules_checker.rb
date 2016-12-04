require "ipaddress"
require_relative 'utils/custom_file_utils.rb'

class SemanticRulesChecker
    def check(context, topology_provider)
        @context = context
        @topology_provider = topology_provider
        @topology = @topology_provider.initial_topology

        hosts_are_well_defined

        flows_are_well_defined

        values_defined_in_flows_are_defined_in_topology

        paths_for_flows_exists
    end

    def hosts_are_well_defined

    end

    def flows_are_well_defined

    end

    def values_defined_in_flows_are_defined_in_topology
        hosts_in_topology = @topology.select{ |elem| elem.is_a? Host }
        flows_identifiers_defined = @context['identifiers'].select{ |identifier| identifier.value.is_a? HaikunetFlow }

        #If a HaikunetHost is used as parameter of the flow, then semantically is well 
        #constructed, since if the Host is not defined, it will be constructed first. (At this step 
        #we know that if the HaikunetHost is defined, then its already defined in some part
        #of the program)

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
                path = @topology_provider.source_provider.get_path_between one_src, one_dst
                raise_semantic_error "there is no path between the source with mac #{one_src.mac} and the destiny with mac #{one_dst.mac}" if path.links.size == 0
            end
        end        
    end

    def obtain_host_topology_representation_of(host_identifier)
        hosts_in_topology = @topology.select{ |elem| elem.is_a? Host }
        #A host_identifier can be either a property of the host in the topology, or a HaikunetHostIdentifier or an Array!. 
        #In this second case, it can happens that the host is not defined in the initial topology!.
        if host_identifier.is_a? HaikunetIdentifier
            mac_value = host_identifier.value.params.select{ |param| param.name == 'mac' }.first.value
            hosts_in_topology.each do |host|
                return host if host.mac == mac_value
            end
            #If we are here, then it means that the host is not defined in the actual topology!. This means that
            #we have first to define it in the topology, and afterwards check if there is a path between him and
            #the destiny.

            #TODO: Implement the semantic check for hosts that are not yet defined in the topology. Probably the 
            #topology of the topologygenerator gem will have to be more intelligent (transform it to a graph would 
            #be the best solution I think).

        elsif host_identifier.is_a? String #Is a property of a host 
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

    def raise_semantic_error(message)
        raise SemanticalError, "A semantic error was found in one of the flow definitions. The problem is that #{message}\n Please correct this error in order to run the program ;)."
    end
end