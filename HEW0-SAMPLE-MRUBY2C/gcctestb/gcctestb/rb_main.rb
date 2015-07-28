def handler
	p "handler here"
	if @toggle == 0 then
		@toggle = 1
		p "0 call"
		Kernel::Skiponce_action()
		puts  "0 call end"
	else
		@toggle = 0
		p "1 call"
		Kernel::Skiponce_action()
		p "1 call end"
	end
end

  GC.interval_ratio = 20
  GC.step_ratio = 200


  @toggle = 0
  Kernel::Skiponce_init()

  puts "mruby Hello!"

 