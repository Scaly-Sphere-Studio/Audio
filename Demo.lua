print("Demo.lua start");

print("Volume:", Audio.getVolume())
print("Device:", Audio.getDevice())

do
  local _ENV = Audio
  
  Buffer.get(0):loadFile("sounds/bat-la-rate.mp3")
  Buffer.get(1):loadFile("sounds/ok.mp3")
  
  local source = Source.get(0)
  --source.loop = true;
  source:queueBuffers({0, 1})
  --source.volume = 30
  source:play()
  
  while (source.is_playing)
  do
  end
end

print("Demo.lua end");