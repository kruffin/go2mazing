## What is it?
A fun<sub>*</sub> little maze game for the ODroid Go Advance.

<sub>*</sub> opinion of its creator; he would even say a mazing game.

![Go 2 Mazing Lvl 1](/images/lvl_1_web.jpg)
![Go 2 Mazing Lvl 10](/images/lvl_10_web.jpg)

### Story
![Candle1](/images/candle_11_front.png)
![Candle2](/images/candle_boy_11_front.png)
![Candle3](/images/candle_girl_11_front.png)
You are on a table, waiting your turn to be placed upon the cake. As the other candles are chosen, you become anxious. The cake is picked up and you are left on the table with a few others. Not being picked saddens you.

It's not right.

It's not fair.

Cakes are there for you to sit upon and shine your wonderful light. You will have your cake and you will sit upon it proudly.

The like-minded unchosen candles agree and will join you on your quest. Thus begins the/a mazing quest to catch a cake and grace it with your posterior.

Will you be placed in a spot of honor front and center for all to see or on the edges, easily forgotten? Only the future knows and history decides.

Begin.

## Build
Pull down the submodules.

    git submodule update --init --recursive

### Prereqs
There are two ways to build this program:

1. To run directly on the Go Advance.
2. To run on a GLFW compatible computer.

#### 0. Common
The following packages are required:

    build-essential libopenal-dev libpng-dev

If running on Ubuntu 18 (the current builds from HardKernel are) then a newer gcc is required (see here for more information - https://linuxize.com/post/how-to-install-gcc-compiler-on-ubuntu-18-04/):

    sudo apt install software-properties-common
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt install gcc-9 g++-9
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9 --slave /usr/bin/gcov gcov /usr/bin/gcov-9

#### 1. Go Advance Prereqs
This requires the libgo2 library from: https://github.com/OtherCrashOverride/libgo2 It should already be on the ODroid Go Advance.

To build the application run:

    make go2

#### 2. GLFW
Requires the additional package:

    libglfw3-dev

To build run:

    make glfw

## Install on Go Advance

Clone the repo and copy over to the go advance (or clone directly on the go advance). Do the build.

Now there are two options to get it into the carousel to run:

1. An additional program within the settings gear menu.
2. Add another system into the carousel just for this and potentially other programs.

### Option 1 - Existing Settings Gear

Put a shell script in /opt/system/ called something like `go2mazing.sh` with the path to the built go2radio binary.

    /home/odroid/src/go2mazing/go2mazing

Make the script executable:

    chmod +x /opt/system/go2mazing.sh

You can now run it from the gear menu like the network app and others.

### Option 2 - Add another Gear
This option is really only necessary if you want to alter how the programs run. Some emulators turn the performance to max, run itself and then turn it back to normal. If you want a mazing experience, set the perfermance to max!

Make a copy of the /etc/emulationstation/es_systems.cfg

    cp /etc/emulationstation/es_systems.cfg /home/odroid/.emulationstation/.

Then add an additional system at the end still inside the `<systemList> </systemList>` tags.

    <system>
            <name>retropie</name>
            <fullname>Apps</fullname>
            <path>/home/odroid/apps</path>
            <extension>.sh</extension>
            <command>perfmax;%ROM%;perfnorm</command>
            <platform></platform>
            <theme>retropie</theme>
    </system>

Note the path `/home/odroid/apps` and create it.

Next create a script like in Option 1, but place it inside the `/home/odroid/apps/` folder instead.

There will be an additional gear menu that has the program in it.