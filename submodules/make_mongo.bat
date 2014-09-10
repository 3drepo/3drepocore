cd mongo-cxx-driver
scons -j2 --full --64 --mute --sharedclient --dynamic-windows --use-system-boost --extrapath=c:\local\boost_1_56_0 --cpppath=c:\local\boost_1_56_0 --libpath=c:\local\boost_1_56_0\lib64-msvc-12.0 install-mongoclient
scons -j2 --full --64 --mute --sharedclient --dynamic-windows --use-system-boost --extrapath=c:\local\boost_1_56_0 --cpppath=c:\local\boost_1_56_0 --libpath=c:\local\boost_1_56_0\lib64-msvc-12.0 --dbg=on --opt=on install-mongoclient
