require 'typhoeus'
require_relative '../errors/onos_code_generator_error.rb'

module OnosCodeGenerator
    include CustomFileUtils
    
    def generate_output(file_name)
        code = ''
        requests = []
        json = ''
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost
                host_params = get_host_params identifier
                next if is_defined_in_topology host_params
                json = "{
                    \"mac\" : \"#{host_params['mac']}\", \n
                    \"vlan\" : \"#{host_params['vlan']}\", \n
                    \"ipAddresses\" : #{host_params['ips']}, \n
                    \"location\" : {
                        \"elementId\" : \"#{host_params['elementId']}\", \n
                        \"port\" : \"#{host_params['port']}\"
                    }
                }\n"
                requests.push({ "end_point" => "hosts", "message" => json })
                code += json
            when HaikunetFlow
                src_endpoint = ''
                dst_endpoint = ''
                flow_params = get_flow_params identifier
                flow_params['src'].each do |src_mac|
                    src_endpoint += "{\"name\":\"#{src_mac}\"},"
                end
                src_endpoint.chomp

                flow_params['dst'].each do |dst_mac|
                    dst_endpoint += "{\"name\":\"#{dst_mac}\"},"
                end
                dst_endpoint.chomp

                json = "
                    {
                    \"intent:intent\":
                        {
                            \"intent:id\":\"b9a13232-525e-4d8c-be21-cd65e3436034\",
                            \"intent:action\": [ { \"order\" : 2, \"allow\" : {} } ],
                            \"intent:subjects\" : [ { \"order\":1 , \"end-point-group\" : #{src_endpoint} }, { \"order\":2 , \"end-point-group\" : #{dst_endpoint} } ]  
                        }
                    } 
                    "
                requests.push({ "end_point" => "b9a13232-525e-4d8c-be21-cd65e3436034", "message" => json })
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

=begin

curl -v --user "admin":"admin" -H "Accept: application/json" -H "Contennt-type: application/json" -X GET http://localhost:8181/restconf/config/intent:intents/intent


curl -v --user "admin":"admin" -H "Accept: application/json" -H "Content-type: application/json" -X PUT 

http://localhost:8181/restconf/config/intent:intents/intent/b9a13232-525e-4d8c-be21-cd65e3436034 -d '
{ 
  "intent:intent" : 
    { "intent:id": 
      "b9a13232-525e-4d8c-be21-cd65e3436034", 
        "intent:actions" : [ { "order" : 2, "allow" : {} } ], 
        "intent:subjects" : [ { "order":1 , "end-point-group" : {"name":"10.0.0.1"} }, { "order":2 , "end-point-group" : {"name":"10.0.0.2"}} ] 
    } 
}'

=end



    def value_from(value_name, array)
        elem = array.select { |elem| elem.name == value_name }.first
        elem.nil? ? elem : elem.value
    end

    def get_host_params(host_identifier)
        mac = value_from 'mac', host_identifier.value.params
        vlan = value_from 'vlan', host_identifier.value.params
        ips = value_from 'ipAddresses', host_identifier.value.params
        elementId = value_from 'elementId', host_identifier.value.params
        port = value_from 'port', host_identifier.value.params

        {'mac' => mac, 'vlan' => vlan || -1, 'ips' => ips || '[]', 'elementId' => elementId || '', 'port' => port || ''}
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

    def is_defined_in_topology(host_params)
        uri_resource = 'http://127.0.0.1:8181/onos/v1'
        response = Typhoeus.get "#{uri_resource}/hosts/#{CGI.escape(host_params['mac'])}/#{host_params['vlan']}", userpwd:"onos:rocks"
        response.code == 200
    end
end