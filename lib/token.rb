class Token
	attr_reader :keyword, :value
	def initialize(keyword,value)
		@keyword = keyword
		@value = value
	end
end