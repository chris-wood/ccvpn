# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ivan/Desktop/projects/ccvpn/ccvpn

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ivan/Desktop/projects/ccvpn/ccvpn/b

# Include any dependencies generated for this target.
include gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/depend.make

# Include the progress variables for this target.
include gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/progress.make

# Include the compile flags for this target's objects.
include gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/flags.make

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/flags.make
gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o: ../gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ivan/Desktop/projects/ccvpn/ccvpn/b/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o"
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o   -c /home/ivan/Desktop/projects/ccvpn/ccvpn/gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore.c

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.i"
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ivan/Desktop/projects/ccvpn/ccvpn/gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore.c > CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.i

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.s"
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ivan/Desktop/projects/ccvpn/ccvpn/gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore.c -o CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.s

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.requires:

.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.requires

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.provides: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.requires
	$(MAKE) -f gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/build.make gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.provides.build
.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.provides

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.provides.build: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o


# Object files for target test_athena_LRUContentStore
test_athena_LRUContentStore_OBJECTS = \
"CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o"

# External object files for target test_athena_LRUContentStore
test_athena_LRUContentStore_EXTERNAL_OBJECTS =

gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/build.make
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: gateway/ccnx/forwarder/athena/libathena.a
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/liblongbow.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/liblongbow-textplain.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /usr/lib/x86_64-linux-gnu/libevent.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /usr/lib/x86_64-linux-gnu/libssl.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /usr/lib/x86_64-linux-gnu/libcrypto.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libccnx_api_portal.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libccnx_transport_rta.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libccnx_api_control.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libccnx_api_notify.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libccnx_common.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: /home/ivan/Desktop/projects/CCNx_Distillery/usr/lib/libparc.so
gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ivan/Desktop/projects/ccvpn/ccvpn/b/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_athena_LRUContentStore"
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_athena_LRUContentStore.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/build: gateway/ccnx/forwarder/athena/test/test_athena_LRUContentStore

.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/build

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/requires: gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/test_athena_LRUContentStore.c.o.requires

.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/requires

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/clean:
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test && $(CMAKE_COMMAND) -P CMakeFiles/test_athena_LRUContentStore.dir/cmake_clean.cmake
.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/clean

gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/depend:
	cd /home/ivan/Desktop/projects/ccvpn/ccvpn/b && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ivan/Desktop/projects/ccvpn/ccvpn /home/ivan/Desktop/projects/ccvpn/ccvpn/gateway/ccnx/forwarder/athena/test /home/ivan/Desktop/projects/ccvpn/ccvpn/b /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test /home/ivan/Desktop/projects/ccvpn/ccvpn/b/gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gateway/ccnx/forwarder/athena/test/CMakeFiles/test_athena_LRUContentStore.dir/depend

