# MBI5124 ghost pixels problem 

Custom 32x32 LED panels with reported problem that occasionally a block of 8 LEDs would get stuck in a state where one was dark and the other 7 were dimmly lit.

![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/76422ef1-c4b5-493b-9db3-92d6fcad9c8f)

Problem turned out to be that with the default pre-charge voltage, the LEDs were being reverse biased at higher than thier breakdown voltage when OFF. Eventually one of the LEDs (the one with the lowest breakdown voltage likely due to process variaions) would go into breakdown and start conducting backwards. The reverse current flows from the OUT pin, though the broken down LED, and then forward though the other LEDs in the column down to ground though the delselected row transistors...

![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/010470e0-7021-46a4-8e89-963429c92200)

# The solution

It turns out that it is possible to adjust the pre-charge voltage on this IC using the barely documented `configuration register`....

![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/e12e5122-526f-4ee6-bbb6-46c7630fdfd5)

By setting this configuration register to [the value to 0x0019](reg-value-scan.md), we reduce the pre-charge voltage low enough that none of the LEDs go into breakdown. In my case, the voltage went from about 3V down to about 2V.

To set the register, you clock out the new 16 bit value to all of the chips using the normal SDA and CLK lines, then you raise LAT and count out 4 pulses on CLK and then lower LAT. Yep. 

# Implmentation on NovaCLT

Luckily, NovaCLT knows about this chip an exposes a way to set this register. Make sure your module uses the MBI5124 chip type, and then go into Screen Config->Receiver Board->More Settings...

![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/72bda334-ee40-4fb3-b7e2-eae0e4fde446)

Here are the files for my setup (in this repo)...

MBI 32x64 Works Low Precharge.rcfgx

Custom MBI5124 32x32 S8 G2 Low Precharge.module

# Complications

In my case, some of the LEDs had been damaged from the reverse current while this problem was happening. These LEDs now permentently have a greatly reduced reverse breakdown voltage and so now continously leak a steady current in the reverse direction whenever not lit. Here is the current path from the precharger on column C0 (OFF), through the damged LED, and though through R1 to ground on unselected row R1...

![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/2760d024-7ace-43a1-8dd6-dc17b34ab095)

When the pre-charge voltage is dropped, current can now flow from the active row, though a good LED, through an OFF column connection, and then leak through the damaged LED to ground through an inactive row.

In this image L1 is the damaged LED that leaks reverse current ROW0 is selected, ROW1 is unslected, and OUT0 is OFF...
![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/0900cdee-3187-40a1-8d61-42f2348c546b)

To mitigate this problem, you can try finding a pre-charge voltage that is a comprimise- low enough to avoid good LEDs from going into reverse breakdown but high enough to reduce the backwards leakage current enough that it does not visible light good LEDs. But if it was me, I'd want to replce those damaged LEDs since they are likely going to age badly.

# Handly Tool

Here is a handy little tool that runs on an Arduino that lets you easily play around with indivial signals and quickly test very specific conditions out. I hope you like it.

[MBI5124-Arduino-fiddler](MBI5124-Arduino-fiddler)




 
