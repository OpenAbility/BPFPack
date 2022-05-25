# BPFPack
A tool for packing a directory into BPF files

## Usage
The command line tool is a single-purpose with no options at all.
It is structured as such:
```sh
bpfpack <DIR_NAME> <FILE_NAME>
```
But in this case `<FILE_NAME>` is actually optional, and will automatically convert to `<DIR_NAME>.bpf`.  
What it does it that it gets all **top level**(not recursive) files of the directory specified and packs them into a `.bpf` file.  
The tool isn't too evolved, but it is used in a W.I.P game and will most likely be updated with recursive support.

Written by *khhs*, protected by the *MIT License 2*​
14
Written by *khhs*, protected by the *MIT License 2
