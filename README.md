Bug
===

An operating system

Building and Development
-----------

Bug is a Qt Creator project. In order to build it, qmake (or Qt Creator) must be available. Several other tools, such as cross compilers and x86 emulators are required as well. On a Debian-based distro, `scripts/install-build-tools.sh` is provided to install all the required build tools for building Bug OS.

Once installed, simply opening `Bug.pro` within Qt Creator and building the project is enough.

Running
-------

Running an OS requires a special execution environment, i.e. an emulator such as QEMU. Unfortunately, special run configurations can't be stored in a shared config file. In order to run the project from Qt Creator (by pressing the green button) a new configuration has to be created. This is done by going to `Projects -> Desktop (Run) -> Run -> Add`, and specifying the following as the executable:

    %{sourceDir}/scripts/run-qemu.sh
    
![Qt Creator Run Configuration](http://i.imgur.com/pjLQG5M.gif)

Debugging
---------
