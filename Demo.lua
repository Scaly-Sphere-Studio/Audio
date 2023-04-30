print("Demo.lua start");

print("Volume:", Audio.getVolume())
print("Device:", Audio.getDevice())

do
  local _ENV = Audio
  
  init()
  
  buff1 = Buffer.new("sounds/bat-la-rate.mp3")
  buff2 = Buffer.new("sounds/ok.mp3")
  
  source = Source.new()
  --source.loop = true;
  source:queueBuffers({buff1.id, buff2.id})
  source.volume = 30
  source:play()
  
  while (source.is_playing)
  do
  end

  terminate()
end

print("Demo.lua end");