class Mythread_gc < Ssp_thread
	def initialize(id,led)
		super("thread",id)
		@led = led
		@tout   = 1000
	    status = 0
	end
	def settout(tout)
		@tout = tout
	end
	def thread(n)
		@n = n
		while 1 do
			puts "thread here " + @n.to_s
			p "goto delay##################################"
			begin
				Kernel::Ssp_delay(@tout)
			rescue
				p "ssp_delay exception"
			end
#			GC.start
#			puts n.to_s + "GC Finish!"
		end
	end
end
class Mythread_gc2 < Ssp_thread
	def initialize(id,led)
		super("thread",id)
		@led = led
		@tout   = 1000
	    status = 0
	end
	def settout(tout)
		@tout = tout
	end
	def thread(n)
		@n = n
		while 1 do
			puts "thread here " + @n.to_s
			p "goto delay**********************************"
			begin
				Kernel::Ssp_delay(@tout)
			rescue
				p "ssp_delay exception"
			end
#			GC.start
#			puts n.to_s + "GC Finish!"
		end
	end
end


  GC.interval_ratio = 20
  GC.step_ratio = 200

#  Kernel::Ssp_delay(1000)

  led0    = Rx_gpio.new(Rx_gpio::LED0)
  led1    = Rx_gpio.new(Rx_gpio::LED1)

  led0.off
  led1.off

  thread_0   = Mythread_gc.new(2,led0)
  thread_1   = Mythread_gc2.new(3,led1)


  thread_0.settout(300)
  thread_1.settout(500)


  puts "mruby setup finish!"

  thread_0.act
  thread_1.act
 