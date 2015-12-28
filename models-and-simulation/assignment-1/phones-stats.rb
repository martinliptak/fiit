require "./calendar.rb"

All = 10

cal = Calendar.new(:maxtime => 43_200)
    
20.upto(60) { |maxN|
  all_free = 0
  all_calls = 0
  all_attempts = 0

  All.times {
    cal.reset
  
    sum_free = 0
    calls = 0
    attempts = 0
    free = maxN

    event_hang = lambda {
      free += 1
    }

    event_call = lambda {
      attempts += 1

      if free > 0
        free -= 1
        cal.enqueue(event_hang, :exponential => 240)
        
        calls += 1
        sum_free += free
      end
    }

    event_call_a = lambda {
      cal.enqueue(event_call)
      cal.enqueue(event_call_a, :exponential => 10)
    }

    event_call_b = lambda {
      cal.enqueue(event_call)
      cal.enqueue(event_call_b, :exponential => 12)
    }

    cal.enqueue(event_call_a)
    cal.enqueue(event_call_b)

    cal.loop
    
    all_free += sum_free.to_f / calls 
    all_calls += calls
    all_attempts += attempts;
  }
  
  puts "#{maxN} #{all_free / All} #{all_calls / All} #{all_attempts / All}"
}
