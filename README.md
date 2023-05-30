# Dynplug

Dynamic audio plugin.

Dynplug is an audio plugin that can load another audio plugin at runtime without being reloaded in the daw. The default output is silence.

Dynplug is available as VST3 (for now).
Dynplug can load Yaaaeapa plugins (for now).

- On startup, dynplug creates, if not existing, a named pipe "dynplug_magicpipe" in the default OS tempdir (e.g. /tmp/ in most unix systems)
- It continuously reads from magicpipe expecting a string representing a path to the yaaaeapa plugin to be loaded
- It loads the yaaaeapa module via dlmopen
- Dynplug VST3 informs the DAW about the new parameters names (not every DAW correctly supports this feature)
- It executes the module
- On new module loading, it unloads the old one and deletes the file

## Compilation & Installation

```
cd src/vst3
make
make install-user
```

## Execution

Load it in any VST3 supporting DAW.

