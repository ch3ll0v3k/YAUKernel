cmake_minimum_required(VERSION 3.4)

project(YAUKernel)

# CFLAGS
set(CMAKE_C_FLAGS "-Wall -Werror -m32 -O2 -nostdlib -nodefaultlibs")
set(CMAKE_EXE_LINKER_FLAGS "-T ${PROJECT_SOURCE_DIR}/linker.ld")
# nasm support
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)
# output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

# in order to make the .iso file
add_custom_target(
	iso
	COMMAND mkdir -p ${CMAKE_BINARY_DIR}/isodir/boot/grub
	COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel ${CMAKE_BINARY_DIR}/isodir/boot/
	COMMAND cp ${PROJECT_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/isodir/boot/grub
	COMMAND grub-mkrescue -d /usr/lib/grub/i386-pc/ -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/YAUK.iso ${CMAKE_BINARY_DIR}/isodir
	DEPENDS kernel
)
