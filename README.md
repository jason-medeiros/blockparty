# libblockparty: blockparty multipurpose library.  

This library contains a lot of useful code developed over my career working as a penetration tester.  The vast majority of the code within this library is designed to extend my own capabilities, as well as provide a base of code from which I can develop useful tools or applications.  This library is fairly large, and was developed privately over a long period of time.  This library contains it's own event system, x86-32 disassembler, file format parsers, memory profiler, network bindings, database bindings, and a variety of other useful code.  I had initially designed it with the idea that I would one day sell the code, but since it has become too large for me to manage on my own, I've decided to simply open source it under the MIT license so someone can potentially find use from it.  I will be updating the library periodically. The library is named after the Pacific Beach, San Diego block party, where I first had the idea for the library in 2006.


# Building this library:

The library utilizes autotools.  A frontend script for building/installing the library can be found in ./library/BuildAndInstallBlockparty.sh

You should be able to simply run the script on most platforms to install it.  

Note: You must install dependencies BEFORE building the project.  


# Installing Dependencies:

To install library dependencies, please open and read the script:

./dependencies/blockparty_install_deps_ubuntu16.04.sh

important: Read it before running it, as it doesn't do any error checking when doing dependency installing and you will probably have a better time if you know what the script is supposed to do.  

The full current dependencies can be installed via apt with:
apt-get install -y gcc g++ make cmake
apt-get install -y libevent-dev
apt-get install -y libxml2-dev libssl-dev libcurl4-openssl-dev libvmime-dev libpcap-dev libmailutils-dev libglib2.0-dev libjson-glib-dev libstatgrab-dev libmicrohttpd-dev libmariadb-client-lgpl-dev libmariadb-client-lgpl-dev-compat libmicrohttpd-dev libpcre3-dev libsqlite3-dev

You also need libcsv_parser which is provided as a source package since an apt package isn't available.  The blockparty_install_deps_ubuntu16.04.sh contains instructions on how to build it.

The project also requires libmozjs-31-dev, which you either need to install from source, or via PPA by adding the following repo using the following commands:

add-apt-repository ppa:ricotz/testing
apt-get update
apt-get install libmozjs31-dev



# Examples:

Coming soon.

# Using this library:

You can use this library however you want, but that being stated, this library is a work in progress, and it's been written ENTIRELY by one person.  There may be bugs, where-in-fact, there are bugs.  For example, the websocket implementation is working, but when taking multiple connections, hangs occur.  I'm still working on ironing out bugs, so unless you find a usage case that I've provided that says the code is bug free and ready for production, do not use it for production code.  Also, most of this code is developed in a non-stream oriented way, relying on pre-allocation in order to generate outputs.  What this means is that if you have files that are larger than available ram, it's likely that parsing will fail due to not being able to pre-allocate memory.  This is by design, having come from a C/ASM background.  

The event system was developed using the reactor/observer pattern, without knowing that the reactor/observer pattern exists.  When I first started developing the library, I needed an event system, but didn't completely understand most design patterns.  I designed the blockparty event system by spending a lot of time in front of a video camera and whiteboard, spitballing what I estimated to be the correct implementation based on needs.  Without even knowing what it was, I chose the reactor/observer pattern.  I recommend if you're starting a new project that needs an event system, utilize node.js, as it has a very strong implementation of reactor/observer, and it's what I'm currently developing my proprietary projects on.  That's not to say that the event system in blockparty isn't useful, it's just other solutions are available and better tested.  Again, the event system works, and is very fast, I simply haven't tested it for thousands of hours, only a few hundred.


# Liability Notice:

This code is not guaranteed to work, it could potentially even cause the earth and sun to explode, there is no unit test that proves otherwise.  I did my best with what time I had, and I tried to avoid pitfalls where I could, but do not expect this code to be usable on extremely mission critical places, as I am only one man, and I don't have the resources to survive your lawsuit.
