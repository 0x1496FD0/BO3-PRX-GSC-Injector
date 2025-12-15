# BO3-PRX-GSC-Injector
A Black Ops 3 1.33 prx .gscc injector plugin for GoldHEN working on any jailbreakable firmware

# Installation
  1. Drop the .prx inside __/data/GoldHEN/plugins__
     
  2. Register the .prx for your BO3 CUSA in __/data/GoldHEN/plugins.ini__
     <img alt="image" src="https://8upload.com/image/ddd9c5061d4c0528/610D7634-1B61-48B0-8829-0F705B632C9C.jpeg"/>
  
  3. Drop your __.gscc__ files inside __/data/BO3 PRX GSC Injector__ (the folder is auto created on run if non existent) and rename them accordingly to follow the convention gscc_{n} where __{n}__ is the index of the script (__ex: gscc_0   gscc_1   gscc_2  ...__)
     (yeah i know it suck but the orbis kernel fn to list dir entries fail for some reason)

# Filezilla users
  Be carreful if you rename the .gscc file locally before transfering there is a bug on filezilla who cut 1 byte on your file if it doesnt have an extension on transfer (experienced on arch linux filezilla 3.69.5-2 from the pacman repo but the same bug probably exist for windows and other versions)

# Read this
    Make sure you are in the zm/mp/cp lobby before injecting
    You can add and remove .gscc while in game just make sure to respect the naming convention

# Build dependencies
  https://github.com/GoldHEN/GoldHEN_Plugins_SDK

# Installation/Showcase video
  [![](https://img.youtube.com/vi/L76KVwBi7V8/0.jpg)](https://www.youtube.com/watch?v=L76KVwBi7V8)
