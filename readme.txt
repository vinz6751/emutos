This is a customized EmuTOS version I play with. It's targetted specifically at stock Atari STf/STe hardware, so support from other machines or extensions is removed to make room for goodies.
Original EmuTOS aims at being perfectly TOS compatible, support as much hardware as realisticly possible, and C is prefered whenever possible over asm.
This fork aims at packing as much as possible into 256K ROM, compatibility to games/demos will be sacrificed (original ROMs are so easy to find and are perfect for that purpose), as the OS aims at being useful if the Atari is used as "main computer" for programming.

Full credits to the EmuTOS team for making great EmuTOS and for allowing such customizations.
See readme-mps.txt for more details on differences between EmuTOS and this fork.

This for has the following improvements:
1. The ACCPATH environment variable allows to specify the folder from which GEM accessories will be loaded (it doesn't have to be the boot drive's root anymore).
2. You can compile the OS with MPS_BLITTER_ALWAYS_ON=1 so it always uses the blitter for drawing. The "no-blitter" drawing routines will not be included and it saves ROM space for other features.
3. A small sound is played at cold boot and warm boot, they're are a bit different.
4. The BIOS injects "the_env" OS variable to programs it executes, including the AES. So you if you sent that variable to something you want, it will actually have an effect. This was used so we can set the PATH environment variable of EmuCON, or ACCPATH of the AES.


Refactorings
1. The AES configuration file is moved to a separate file (aescfg.c).
2. Supporting functions shared between the graphics shell and EmuCON are in shellutl.c
3. The logo displayed at startup is compressed using tools/logo_compressor.c. The compression is basic but it helps a bit.
4. The startup of the BDOS is now in bootstrap.c so it's better separated. Code is moved from bios/initinfo.c is renamed into bdos/initinfo.c because at the time it runs, the BDOS is effectively running and BDOS functions are used for the boot screen. In that file, there are now helper functions like crlf() so to avoid code repetition and save ROM space.
5. iumem is reworked (CONF_WITH_NON_RELOCATABLE_SUPPORT) so to support non-relocatable programs (ie programs that want to be located at a particular location in RAM). This was done because I wanted to use the Calypsi compiler, but it could not produce TOS executables, only PGX/PGZ which are not relocatable. But this is probably not needed anymore as version 3.7 can produce TOS executables.
6. kpgmld.c/proc.h are reorganized and broken up so it only orchestrates the loading of files to execute. kpgmld.c use program loaders and adapts to proc.c The reading of files (from disk or ROM) is done by a program_reader.c and the identification/relocation etc. done by a program_loader. This was also done for sake of being able to use Calypsi and is probably not needed anymore functionnaly, but it's good to have more separated unit of codes having different responsibilities so the design can stay.
7. BIOS's tries to have a bit more single reponsibility in managing memory. For example, xmaddalt is replaced with bmem_register.


----
The below is the original EmuTOS readme:

This release has been built on Linux Mint (a Ubuntu derivative), using
Vincent Rivière's GCC 4.6.4 cross-compiler.  The custom tools used in
the build process were built with native GCC 4.8.4.

The source package and other binary packages are available at:
https://sourceforge.net/projects/emutos/files/emutos/1.2.1/

An online manual is available at the following URL:
https://emutos.github.io/manual/

The extras directory (if provided) contains:
(1) one or more alternate desktop icon sets, which you can use to replace
    the builtin ones.  You can use a standard resource editor to see what
    the replacement icons look like.
    To use a replacement set, move or rename the existing emuicon.rsc &
    emuicon.def files in the root directory, then copy the files containing
    the desired icons to the root, and rename them to emuicon.rsc/emuicon.def.
(2) a sample mouse cursor set in a resource (emucurs.rsc/emucurs.def).  This
    set is the same as the builtin ones, but you can use it as a basis to
    create your own mouse cursors.
    To use a replacement set, copy the files containing the desired mouse
    cursors to the root, and rename them to emucurs.rsc/emucurs.def.
For further information on the above, see doc/emudesk.txt.

If you want to read more about EmuTOS, please take a look at these files:

doc/announce.txt      - Introduction and general description, including
                        a summary of changes since the previous version
doc/authors.txt       - A list of the authors of EmuTOS
doc/bugs.txt          - Currently known bugs
doc/changelog.txt     - A summarised list of changes after release 0.9.4
doc/emudesk.txt       - A brief guide to the newer features of the desktop
doc/incompatible.txt  - Programs incompatible with EmuTOS due to program bugs
doc/license.txt       - The FSF General Public License for EmuTOS
doc/status.txt        - What is implemented and running (or not yet)
doc/todo.txt          - What should be done in future versions
doc/xhdi.txt          - Current XHDI implementation status

Additional information for developers (just in the source archive):

doc/install.txt       - How to build EmuTOS from sources
doc/coding.txt        - EmuTOS coding standards (never used :-) )
doc/country.txt       - An overview of i18n issues in EmuTOS
doc/fat16.txt         - Notes on the FAT16 filesystem in EmuTOS
doc/memdetect.txt     - Memory bank detection during EmuTOS startup
doc/nls.txt           - How to add a native language or use one
doc/old_changelog.txt - A summarised list of changes up to & including
                        release 0.9.4
doc/osmemory.txt      - All about OS internal memory in EmuTOS
doc/reschange.txt     - How resolution change works in the desktop
doc/resource.txt      - Modifying resources in EmuTOS
doc/startup.txt       - Some notes on the EmuTOS startup sequence
doc/tos14fix.txt      - Lists bugs fixed by TOS 1.04 & their status in EmuTOS
doc/version.txt       - Determining the version of EmuTOS at run-time

The following documents are principally of historical interest only:

doc/old_code.txt      - A museum of bugs due to old C language
doc/vdibind.txt       - Old information on VDI bindings

-- 
The EmuTOS development team
https://emutos.sourceforge.io/
