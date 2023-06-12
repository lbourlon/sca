# Side Channel Attacks

University project, to explore CPA attacks on AES.
The traces come from a ChipWhisperer AES implementation.

Figured I'd make the implementation public, I'll write a bit more about it here later.

It had has a "normal" mode "online" mode (which is quite a bit faster).
And a "test" mode which I don't quite remember what's for.

# Clone Repo Unpack traces and compile :

```
git clone https://github.com/lbourlon/sca
tar -xzf traces.tar.gz && make
```

# usage : 
```
./prog <traces_file> <textes_file> <keys_file> <normal|o
nline|test> <start_trace> <end_traces> <nb_echant>
```
```
./prog traces.raw textes.raw keys.raw online 32 279 870

```
