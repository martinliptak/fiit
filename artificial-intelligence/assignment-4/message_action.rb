class MessageAction < Action
	attr_reader :messages

	def initialize(*messages)
		@messages = messages
  end

  def execute(dictionary)
    puts "Sprava: #{@messages.map { |o| dictionary[o] || o }.join}"
  end
	
	def to_s
		"(message #{@messages.join(" ")})"
	end
end
