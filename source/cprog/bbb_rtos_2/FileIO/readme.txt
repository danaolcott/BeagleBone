10/2017

Access to sd card assumes that the card is formatted and mounted.  The 
drive / folder access is /media.  

To Mount the SDCard:

Insert the card
Read the parition name
	Type: lsblk

Type: mount /dev/mmcblk0p1 /media

This creates a folder called /media at the top level
Access it as you would any other folder.

