#!/usr/bin/env ruby

require 'cgi'
require 'json'
require 'typhoeus'
require 'byebug'

uri_resource = 'http://127.0.0.1:8181/onos/v1/'
flows_resource = 'flows'

response = Typhoeus.get "#{uri_resource}#{flows_resource}", userpwd:"onos:rocks"
flows_ids_per_device = (JSON.parse response.body)['flows'].map { |flow_json| {'id' => flow_json['id'], 'deviceId' => flow_json['deviceId'] } }
flows_ids_per_device.each do |flow_id_and_device| 
	Typhoeus.delete "#{uri_resource}#{flows_resource}/#{CGI.escape(flow_id_and_device['deviceId'])}/#{flow_id_and_device['id']}", userpwd:"onos:rocks"
end