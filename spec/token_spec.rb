require 'spec_helper'
require 'token.rb'

describe Token do
  context 'when no parameter is recieved' do
    it "raise an ArgumentError" do
      expect { Token.new  }.to raise_error ArgumentError
    end
  end
  context 'when keyword and value are recieved' do
    before :each do
      @my_token = Token.new "my_keyword", "my_value"
    end

    it "creates a valid Token" do
      @my_token.class == Token
    end
    
    it "returns the keyword which was used for creation" do
      @my_token.keyword == "my_keyword"
    end
    
    it "returns the value which was used for creation" do
      @my_token.value == "my_value"
    end
  end
end