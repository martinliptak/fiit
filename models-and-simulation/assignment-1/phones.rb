require "./calendar.rb"

MaxN = 30

time = Time.now
puts "Zaciatok simulacie"

cal = Calendar.new(:maxtime => 43_200)
free = MaxN
sum_free = 0
calls = 0
attempts = 0

event_hang = lambda {
  puts "#{cal.simtime.round(3)} Zavesil"
  free += 1
}

event_call = lambda {
  attempts += 1

  if free > 0
    puts "#{cal.simtime.round(3)} Vola"
    
    free -= 1
    cal.enqueue(event_hang, :exponential => 240)
    
    calls += 1
    sum_free += free
  end
}

event_call_a = lambda {
  puts "#{cal.simtime.round(3)} Ide volat A"
  cal.enqueue(event_call)
  cal.enqueue(event_call_a, :exponential => 10)
}

event_call_b = lambda {
  puts "#{cal.simtime.round(3)} Ide volat B"
  cal.enqueue(event_call)
  cal.enqueue(event_call_b, :exponential => 12)
}
 
cal.enqueue(event_call_a)
cal.enqueue(event_call_b)

cal.loop

puts "Koniec simulacie (simulacia trvala #{Time.now - time}s)"
puts
puts "Priemerny pocet volnych liniek: #{sum_free.to_f / calls}"
puts "Priemerna vyuzitie linky: #{(100 - (sum_free.to_f / calls) * 100 / MaxN).round(2)}%"
puts "Pokusy o spojenie: #{attempts}"
puts "Spojenia: #{calls}"
puts "Spojenia na pokusy: #{calls * 100 / attempts}%"
