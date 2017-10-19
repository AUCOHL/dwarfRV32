set_property PACKAGE_PIN W5 [get_ports {clk}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {clk}]
	create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk]

set_property PACKAGE_PIN R2 [get_ports {rst}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {rst}]

set_property PACKAGE_PIN T1 [get_ports {INTi}]
        set_property IOSTANDARD LVCMOS33 [get_ports {INTi}]
        
set_property PACKAGE_PIN U1 [get_ports {mode[1]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {mode[1]}]
        
set_property PACKAGE_PIN W2 [get_ports {mode[0]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {mode[0]}]


set_property PACKAGE_PIN V15 [get_ports {in[5]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[5]}]

set_property PACKAGE_PIN W15 [get_ports {in[4]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[4]}]
        
set_property PACKAGE_PIN W17 [get_ports {in[3]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[3]}]
        
set_property PACKAGE_PIN W16 [get_ports {in[2]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[2]}]
        
set_property PACKAGE_PIN V16 [get_ports {in[1]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[1]}]
        
set_property PACKAGE_PIN V17 [get_ports {in[0]}]
        set_property IOSTANDARD LVCMOS33 [get_ports {in[0]}]
        
        
set_property PACKAGE_PIN W7 [get_ports {Out[6]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[6]}]
				
				
set_property PACKAGE_PIN W6 [get_ports {Out[5]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[5]}]
				
set_property PACKAGE_PIN U8 [get_ports {Out[4]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[4]}]
				
set_property PACKAGE_PIN V8 [get_ports {Out[3]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[3]}]
				
set_property PACKAGE_PIN U5 [get_ports {Out[2]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[2]}]
				
set_property PACKAGE_PIN V5 [get_ports {Out[1]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[1]}]
				
set_property PACKAGE_PIN U7 [get_ports {Out[0]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Out[0]}]
	
set_property PACKAGE_PIN V7 [get_ports {Out[7]}]	
        set_property IOSTANDARD LVCMOS33 [get_ports {Out[7]}]	
				

set_property PACKAGE_PIN W4 [get_ports {Y[3]}]		
	set_property IOSTANDARD LVCMOS33 [get_ports {Y[3]}]
			
set_property PACKAGE_PIN V4 [get_ports {Y[2]}]		
	set_property IOSTANDARD LVCMOS33 [get_ports {Y[2]}]
			
set_property PACKAGE_PIN U4 [get_ports {Y[1]}]	
	set_property IOSTANDARD LVCMOS33 [get_ports {Y[1]}]
			
set_property PACKAGE_PIN U2 [get_ports {Y[0]}]
	set_property IOSTANDARD LVCMOS33 [get_ports {Y[0]}]
			
