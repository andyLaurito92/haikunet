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
                flow_params = get_flow_params identifier
                flow_params['src'].each do |src_mac|
                  flow_params['dst'].each do |dst_mac|
                    json = "{
                          \"type\": \"HostToHostIntent\",
                          \"appId\": \"org.onosproject.ovsdb\",
                          \"priority\": #{flow_params['priority']},
                          \"one\": \"#{src_mac}/-1\",
                          \"two\": \"#{dst_mac}/-1\"
                        }
                      "
                    requests.push({ "end_point" => "intents", "message" => json })
                    code += json
                  end
                end
=begin
This is the flow implemented with HostToHostIntent. 
                code += "{
                          \"type\": \"HostToHostIntent\",
                          \"appId\": \"org.onosproject.ovsdb\",
                          \"priority\": #{flow_params['priority']},
                          \"one\": \"#{flow_params['src']}/-1\",
                          \"two\": \"#{flow_params['dst']}/-1\"
                        }
                      "

Another way, is using PointToPointIntent. The apparently improvmenten here is that 
we have selector and treatment, what can be translated into PERHAPS actions and conditions

                code += "{
                          "type": "PointToPointIntent",
                          "appId": "org.onosproject.ovsdb",
                           "treatment": {
                                "instructions": [
                                      {
                                       "type": "OUTPUT",
                                       "port": "3"
                                      }
                                ],
                                "deferred": []
                          },
                          "selector": {
                            "criteria": [
                              {
                                "type": "ETH_SRC",
                                "mac": "4E:6D:A2:71:18:C9"
                              }
                            ]
                          },
                          "priority": 55,
                          "ingressPoint": {"port": "1","device": "of:0000000000000002"},
                          "egressPoint": {"port": "1","device": "of:0000000000000003"}
                        },


                        {
                          "type": "PointToPointIntent",
                          "appId": "org.onosproject.ovsdb",
                           "treatment": {
                                "instructions": [
                                      {
                                       "type": "OUTPUT",
                                       "port": "3"
                                      }
                                ],
                                "deferred": []
                          },
                          "selector": {
                            "criteria": [
                              {
                                "type": "ETH_SRC",
                                "mac": "5E:B5:79:3D:2A:12"
                              }
                            ]
                          },
                          "priority": 55,
                          "ingressPoint": {"port": "1","device": "of:0000000000000002"},
                          "egressPoint": {"port": "1","device": "of:0000000000000003"}
                        }"

=end
            end
        end

        requests.each do |request|
          response = Typhoeus.post "http://127.0.0.1:8181/onos/v1/#{request['end_point']}",
                        headers: { 'Accept-Encoding' => 'application/json', 'Content-Type' => 'application/json'},
                        body: request["message"],
                        userpwd:"onos:rocks"

          raise OnosCodeGeneratorError, "When posting the following request: #{request['message']} to the endpoint #{request['end_point']}, the following error was raised #{response.body}" unless response.success?
        end

        write_file  "#{file_name[0,file_name.length-3]}_requests",
                    code
    end

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