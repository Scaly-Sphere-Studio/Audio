print("Demo.lua start");

print("Volume:", Audio.getVolume())
print("Device:", Audio.getDevice())

do
  local _ENV = Audio
  
  buff1 = Buffer.create()
  buff2 = Buffer.create()
  buff1:loadFile("sounds/bat-la-rate.mp3")
  buff2:loadFile("sounds/ok.mp3")
  
  source = Source.create()
  --source.loop = true;
  source:queueBuffers({buff1.id, buff2.id})
  --source.volume = 30
  source:play()
  
  while (source.is_playing)
  do
  end
end

print("Demo.lua end");