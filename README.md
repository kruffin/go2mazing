## What is it?
A fun<sub>*</sub> little maze game for the ODroid Go Advance.

<sub>*</sub> opinion of its creator.

## Build
Pull down the submodules.

    git submodule update --init --recursive

### Prereqs
This requires the libgo2 library from: https://github.com/OtherCrashOverride/libgo2 It should already be on the ODroid Go Advance.

It also requires the following packages:

    libopenal-dev

To build the application run:

    make

## Install

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