require 'simple-random'

class Random

  def initialize
    @sr = SimpleRandom.new
    @sr.set_seed(Time.now)
  end
  
  def min_max(min, max)
    min + @sr.uniform * (max - min)
  end
  
  def exponential(mean)
    @sr.exponential(mean)
  end
  
  def fancy(options)
    raise ArgumentError unless options.kind_of?(Hash)
    
    if options.length == 0
      0
    elsif options[:in]
      options[:in]
    elsif options[:exponential]
      exponential(options[:exponential])
    elsif options[:max]
      min_max(options[:min] || 0, options[:max])
    else
      raise ArgumentError
    end
  end
end

