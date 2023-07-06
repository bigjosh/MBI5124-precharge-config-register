I could not find any documentation on the semantics of the MBI5124 pre-charge configuration register, 
so ended up scanning all possible values. I only care about finding low values so I did not 
take the time to figure out the pattern here, but please LMK if you do!

Here are the first 1023 values with OE=0...
![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/9b7b8ef1-b250-4be3-8ed5-0b3d1e230e46)

Here is the full domain with OE=0...
![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/101153d4-77d6-4943-b110-4bee80a66515)

Here are the first 1023 values with OE=1...
![image](https://github.com/bigjosh/MBI5124-precharge-config-register/assets/5520281/c8e4b419-0bc3-499c-9837-b87e354d2f65)

Notice how the first 16 values are now higher? Why?

I picked the value 25 (0x0019) since it is the first lowest value that is low with both OE on and off. 
