cmd_/home/pi/Desktop/ass4.2/modules.order := {   echo /home/pi/Desktop/ass4.2/mydev.ko; :; } | awk '!x[$$0]++' - > /home/pi/Desktop/ass4.2/modules.order
