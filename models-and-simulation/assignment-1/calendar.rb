require 'algorithms'

require './random.rb'

class Calendar
  attr_reader :simtime

  def initialize(options = {})
    @items = Containers::PriorityQueue.new
    @random = Random.new    
    
    if options[:condition]
      @condition = options[:condition]
    end
    @maxtime = options[:maxtime] || 4_000_000_000
    @simtime = 0
  end
  
  def reset
    @simtime = 0
    @items.clear
  end
  
  def enqueue(event, options = {})
    argument = options[:argument]
    options.delete(:argument)
    st = @simtime + @random.fancy(options)
    
    @items.push [event, st, argument], @maxtime - st
  end
  
  def loop
    while running?
      event, @simtime, argument = dequeue
      if argument
        event.call(argument)
      else
        event.call
      end
    end
  end
  
  private
  
  def dequeue
    @items.pop
  end  
  
  def running?
    if @condition
      @condition.call
    else
      @simtime < @maxtime
    end
  end
end


