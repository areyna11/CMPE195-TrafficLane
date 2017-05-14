# CMPE195-TrafficLane
This is the branch in which our final code will be. The code does imaging filtering on a live video feed and on a video file provided. A live demo video can be found [here](https://youtu.be/yjJPKze2-C8)

# Working with the Jetson board
## Prerequisites:
* MUST HAVE your host(Laptop) environment running Ubuntu 14.04, 64 Bit
	* The host needs at least 15-30GB of space to download the necessary files
		* If using Oracle VirtualBox, install the VBox guest additions and the VBox USB controller onto the host VM
* Have a USB cable
* Have the Jetson Tk1 board
* Have a keyboard, mouse, and monitor connected to it
* Have an active internet connection to both devices (host computer(laptop) and the target (Jetson tk1))
* Have an NVidia Developer Account and be signed up to their embedded developer program  (Free)
	* https://developer.nvidia.com/group/node/873376/subscribe/og_user_node?downloadable_file=874988
## SetUp:
1. Download the Jetpack for L4T 2.3 onto the host machine
2. In the directory of where the file was downloaded, run the following commands
	a. chmod +x JetPack-L4T-2.3.run
		i. Use Tab to autocomplete as you type “JetPack”, in order to get the correct and full name. DO NOT USE THE NAME ABOVE
	b. ./JetPack-L4T-2.3.run
		i. Use Tab to autocomplete as you type “JetPack”, in order to get the correct and full name. DO NOT USE THE NAME ABOVE
3. After running the above commands, the JetPack Installer Window should pop up. Once Prompted, select the jetson tk1 board and click next. 
4. Once jetpack shows all the possible downloads, go to the top right of the JetPack installer and click a Full Install
5. Click next. This will download all the files needed for flashing and installing onto the TK1 board. Duration of this step depends on speed of your connection
6. Once downloaded, the window will switch to prepare to flash the board.
7. Follow the onscreen instructions to set up board in Recovery Mode.
	a. In a new terminal, type the command “lsusb”. If a device called “Nvidia Corp” is present in the list, then the board is in recovery mode. 
	b. DO NOT HIT ENTER IN THE “JETPACK” TERMINAL UNTIL YOU HAVE CONFIRMED IN THE OTHER TERMINAL THAT THE BOARD IS IN RECOVERY MODE.
8. Once the board is in recovery mode, hit enter JetPack will begin to the update the device. This may take from 20-45 mins. 
9. Once it installs the OS, it will detect the IP of the target, (TK1). If it's not able to find it, manually enter it.
	a. To get the IP, go to the monitor where the target is connected to. Open the terminal and run the command “ifconfig” Look at the Ip address at “inet addr”. This is the ip
	b. The default login credentials are 
		i. usr: ubuntu
		i. pwd: ubuntu
10. Once the proper information has been entered, click next and a terminal window will pop up. This will ssh into the target and install all the software, such as OpenCV, Cuda libraries and VisionWorks.
11. Then, clone the GitHub code repository: https://github.com/areyna11/CMPE195-TrafficLane

# Execution:
1. After the program has been loaded onto the board, compile and run the program using the commands specified in the source code file.
2. While running the program, the live video feed will be displayed and the video processing will occur automatically. The result after processing and filtering is also displayed on screen.

