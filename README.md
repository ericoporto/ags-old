# Adventure Game Studio

[![Build Status](https://api.cirrus-ci.com/github/adventuregamestudio/ags.svg?branch=ags4)](https://cirrus-ci.com/github/adventuregamestudio/ags)

Adventure Game Studio (AGS) - is the IDE and the engine meant for creating and running videogames of adventure (aka "quest") genre. It has a potential for the other genres as well, although you may find out-of-the-box features lacking.

Originally created by Chris Jones back in 1999, AGS was opensourced in 2011 and since then is continued to be developed by the contributors.

The official homepage of AGS is: http://www.adventuregamestudio.co.uk

Both Editor and Engine are licensed under the Artistic License 2.0; for more details see [License.txt](License.txt).


## Branches and releases

The [master](https://github.com/adventuregamestudio/ags/tree/master) branch is where the next planned version is being developed. It may contain unstable or non fully tested code.

Currently this branch corresponds to 3.\* generation of the engine/IDE and supposed to maintain full backward compatibility with previous releases (see also [Compatibility](#ags-game-compatibility)). According to current plans this branch should only receive improvements to backend, system support and overall perfomance. Changes to data formats and game script should be kept to a strict minimum only necessary to fill in the critical gaps in engine's functionality.

There's a [ags4](https://github.com/adventuregamestudio/ags/tree/ags4) branch also active where we develop a future version currently named simply "ags4". There we introduce greater changes and cut much of the old version support.

According to our plans, at some point in the future "master" branch will be merged with "ags4", while the backward compatible generation will remain as "ags3" branch and only receive fixes and minor enhancements. But there's still some work to do in AGS 3.\*, so exact moment that happens is unknown.

There may be other temporary development branches meant for preparing and testing large changes, but these are situational.

For "official" releases we create "release-X.X.X" branches, that is to prepare the code for the final release and continue making patches to that release if a need arises.
Because of the low number of active developers we tend to only update the one latest release branch. If bugs are found in one of the older versions, then we advise you to update to the latest version first.

Please note that while master branch may contain changes to game data format and new script functions, we cannot guarantee that these will remain unchanged until the actual release. We only formally support data formats and script API that are found in published releases. For that reason it's best to use one of the actual releases if you'd like to make your own game with this tool.


## Building and running

To get started building the AGS engine, see the platform specific instructions or forum threads:

-    [CMAKE](CMAKE.md) - this readme covers centralized build system using CMake
-    [Android](Android/README.md) ([Forum thread](http://www.adventuregamestudio.co.uk/forums/index.php?topic=44768.0))
-    [iOS](iOS/README.md) ([Forum thread](http://www.adventuregamestudio.co.uk/forums/index.php?topic=46040.0))
-    [Linux](debian/README.md) ([Forum thread](http://www.adventuregamestudio.co.uk/forums/index.php?topic=46152.0))
-    [Mac OS X](OSX/README.md) ([Forum thread](http://www.adventuregamestudio.co.uk/forums/index.php?topic=47264.0))
-    [Windows](Windows/README.md)

No longer actively supported, but may work with older code revisions:
-    [PSP](PSP/README.md) ([Forum thread](http://www.adventuregamestudio.co.uk/forums/index.php?topic=43998.0))

On desktop systems launching a game is done by either placing an engine executable in the same directory where the game data is and starting engine up, or by passing game location as a command-line argument.<br>
For Android we have a game launcher app that may run any game, but it's also possible to make a signed APK for your own game (see instructions linked above).<br>
Game configuration is usually found in the acsetup.cfg file. On Windows you may also invoke a setup dialog by running engine with `--setup` argument.<br>
For the list of available config options and command line arguments, please refer to [OPTIONS.md](OPTIONS.md).

AGS Editor is currently only supported on Windows, although it may be run using Wine on Linux and OSX.


## AGS game compatibility

This repository now holds two generations of AGS program, referred to as "AGS 3" and "AGS 4".

**The 3rd generation** of AGS may be currently found in the "master" branch. It's specifics are:

- Supports (imports into the editor and runs by the engine) all versions of AGS games made with AGS 2.50 and up. Note that there may still be compatibility issues with very old games that were not uncovered and fixed yet.
- If you try to run an unsupported game, you will receive an error message reporting original version of AGS it was made in and data format index, which may be used for the reference e.g. when reporting the problem.
- Game saves are compatible between the different platforms if they are created with the same version of the engine. Latest 3.\* engine should normally read saves made by engine 3.2.0 and above, but that has not been tested for a while.
- Certain games may require engine plugins. If there's no plugin version for a particular platform or a platform-independent replacement, then game will not load on that platform.

**The 4th generation** is found in [ags4](https://github.com/adventuregamestudio/ags/tree/ags4) branch. It formally supports importing games made with AGS 3.4.1 and above, but only if these do not use any legacy features.
The 4.x engine should be able to read saves made with the 3.5.0 engine and above.


## Other repositories

There are few other repositories along with the main one, which contain additional resources and may be of interest to you.

- [ags-manual](https://github.com/adventuregamestudio/ags-manual) - holds AGS documentation.
- [ags-templates](https://github.com/adventuregamestudio/ags-templates) - this is where compiled game templates are uploaded in preparation for the Editor's release.
- [ags-template-source](https://github.com/adventuregamestudio/ags-template-source) - this is where template sources are stored and developed (these are essentially AGS game projects).


## Contributing

If you'd like to contribute to the project the usual workflow is this: you fork our repository (unless you already did that), create a feature/fix branch, commit your changes to that branch, and then create a pull request. We will review your commits and sometimes may ask you to alter your code before merging it into our repository.<br>
We may accept patch files too if you send them to the one of the project maintainers.

For bug fixing and trivial code improvements that may be enough, but for more significant changes, completely new features or changes in the program design we ask you to first open an issue in the tracker and discuss it with the development team to make sure that suggested changes won't break anything, nor will be in conflict with existing program behavior or our development plans.

Please be aware that big parts of the engine are still written in a very old and often "dirty" code, and it may not be easy to understand ties between different program parts.
Because there's a low number of active developers involved in this project, our plans or design ideas are not always well documented unfortunately. If you're in doubt - please  discuss your ideas with us first.

The [master](https://github.com/adventuregamestudio/ags/tree/master) branch should be kept in a working state and compilable on all targeted platforms.
The "release-X.X.X" branch is created to prepare the code for respective release and continue making patches to that release. If you've found a critical issue in the latest release it should be fixed in the release-X.X.X branch when possible. Release branch is then either merged to master, or, if that's no longer convenient, - a fix is copied and applied to the master branch separately.

Note that we usually only update the one latest release branch.

Currently the master branch should only receive improvements to existing functionality, perfomance, backend libraries, and so on.<br>
As been mentioned before, there's another branch called [ags4](https://github.com/adventuregamestudio/ags/tree/ags4) where we do bigger changes, cut many of the older version support, and which should eventually become main development branch.
For more information about this split and reasons it was made for please refer to [#448](https://github.com/adventuregamestudio/ags/issues/448).

There's also a Wiki in this repository which serves as a "knowledge base" for this project: https://github.com/adventuregamestudio/ags/wiki<br>
Among other things there we've got a coding convention, please check it before writing the engine code: https://github.com/adventuregamestudio/ags/wiki/AGS-Coding-Conventions-(Cpp)


## Changes from Chris Jones' version of AGS

This version of AGS contains changes from the version published by Chris Jones.
The run-time engine was ported to Android, iOS, Linux, Mac OS X and PSP and a refactoring effort is under way.
A detailed documentation of the changes is provided in the form of the git log of this git repository
(https://github.com/adventuregamestudio/ags).


## Credits

[Link](Copyright.txt)
