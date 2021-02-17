sensor_pin = 0;  --Connect Soil moisture analog sensor pin to A0 of NodeMCU

while true do
  local moisture_percentage = ( 100.00 - ( (adc.read(sensor_pin)/1023.00) * 100.00 ) )
  print(string.format("Soil Moisture(in Percentage) = %0.4g",moisture_percentage),"%")
  tmr.delay(100000);
end

