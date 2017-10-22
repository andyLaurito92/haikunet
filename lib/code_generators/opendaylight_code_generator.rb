require 'typhoeus'
require_relative '../errors/opendaylight_code_generator_error.rb'

=begin

This OpenDayLightCodeGenerator works for Beryllium version. In order to make it work, the OpenDayLight controller
must be running with the following features:

1. feature:install odl-nic-core-mdsal
2. feature:install odl-nic-console
3. feature:install odl-nic-listeners
4. feature:install odl-nic-intent-statemachine
5. feature:install odl-nic-graph
6. feature:install odl-nic-renderer-vtn
7. feature:install odl-netconf-connector-ssh
8. feature:install odl-l2switch-switch-ui

Copying and pasting each line in opendaylight local version should do the trick. We are assuming that both user 
and password are admin.

This is the curl that works which is going to be replicated in this generator:

curl -v --user "admin":"admin" -H "Accept: application/json" -H "Content-type: application/json" -X PUT 
http://localhost:8181/restconf/config/intent:intents/intent/b9a13232-525e-4d8c-be21-cd65e3436033 -d 
'{ "intent:intent" : 
    { 
        "intent:id": "b9a13232-525e-4d8c-be21-cd65e3436033", 
        "intent:actions" : [ { "order" : 2, "allow" : {} } ], 
        "intent:subjects" : [ 
            { "order":1 , "end-point-group" : {"name":"10.0.0.2"} }, 
            { "order":2 , "end-point-group" : {"name":"10.0.0.3"}} 
        ] 
    } 
}' 


=end

module OpendaylightCodeGenerator
    include CustomFileUtils
    
    def generate_output(file_name)
        code = ''
        requests = []
        json = ''
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost
                if !is_defined_in_topology identifier.value
                    raise OpendaylightCodeGeneratorError, "Creating a new Host is not supported by OpenDayLight Code Generator. TODO: Implement this feature."
                end
            when HaikunetFlow
                src_endpoint = ''
                dst_endpoint = ''
                flow_params = get_flow_params identifier
                flow_params['src'].each do |src_mac|
                    src_endpoint += "{\"name\":\"#{src_mac}\"}"
                end
                src_endpoint.chomp

                flow_params['dst'].each do |dst_mac|
                    dst_endpoint += "{\"name\":\"#{dst_mac}\"}"
                end
                dst_endpoint.chomp

                intent_id = "b9a1323#{Random.new.rand 9}-525e-4d#{Random.new.rand 9}c-be21-cd65e343603#{Random.new.rand 9}"
                json = "
                    { \"intent:intent\":
                        {
                            \"intent:id\":\"#{intent_id}\",
                            \"intent:actions\": [ { \"order\" : 2, \"allow\" : {} } ],
                            \"intent:subjects\" : [ 
                                { \"order\":1 , \"end-point-group\" : #{src_endpoint} }, 
                                { \"order\":2 , \"end-point-group\" : #{dst_endpoint} } 
                            ]  
                        }
                    } 
                    "
                requests.push({ "end_point" => "#{intent_id}", "message" => json })
                code += json
            end
        end

        requests.each do |request|
          response = Typhoeus.put "http://localhost:8181/restconf/config/intent:intents/intent/#{request['end_point']}",
                        headers: { 'Accept-Encoding' => 'application/json', 'Content-Type' => 'application/json'},
                        body: request["message"],
                        userpwd:"admin:admin"

          raise OpendaylightCodeGeneratorError, "When posting the following request: #{request['message']} to the following uri http://localhost:8181/restconf/config/intent:intents/intent/#{request['end_point']}, the following error was raised #{response.body}" unless response.success?
        end

        write_file  "#{file_name[0,file_name.length-3]}_requests",
                    code
    end

    def get_flow_params(flow_identifier)
        src = value_from 'src', flow_identifier.value.params
        src_mac = get_macs_from src
        dst = value_from 'dst', flow_identifier.value.params
        dst_mac = get_macs_from dst
        priority = value_from 'priority', flow_identifier.value.params

        {'src' => src_mac, 'dst' => dst_mac, 'priority' => priority}
    end

    def value_from(value_name, array)
        elem = array.select { |elem| elem.name == value_name }.first
        elem.nil? ? elem : elem.value
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

    def is_defined_in_topology(host_identifier)
        return @initial_topology.elements_of_type(Host).select{ |host| host.mac == host_identifier.params.first.value }.length > 0
    end
end