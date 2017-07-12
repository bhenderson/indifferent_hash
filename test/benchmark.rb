require 'benchmark/ips'
require 'indifferent_hash'
require 'ar_hash'

def report(x, msg)
  x.report(msg){ |n| i = 0; while i<n; yield; i+=1 end }
end

objs = [
  # for comparison
  Hash[:foo => :bar, "foo" => :bar],

  ARHash[:foo => :bar],
  IndifferentHash[:foo => :bar],
]

Benchmark.ips do |x|
  objs.each do |obj|
    c = obj.class
    report(x, "#{c} [symbol]"){ obj[:foo] }
    report(x, "#{c} [string]"){ obj["foo"] }
    report(x, "#{c} [both]"  ){ obj[:foo]; obj["foo"] }
  end
end
