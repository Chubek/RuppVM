# RuppVM: Minimalistic Stack VM

I have just recently began work on RuppVM. I am currently specifying the ISA. You can view the stack effects at [ISA.txt](ISA.txt).

A good portion of RuppVM will be generated, as C does not allow much freedom in terms of metaprogramming.

### Features that RuppVM will 'DEFINITELY' have:

- Green threads: These are inter-VM threads, not attached to OS, Kernel, or physical threads. They can be used for co-routines.
- FFI: RuppVM supports FFI via 'hooks' (see [ISA.txt](ISA.txt)].
- Stop-the-world GC: CPython's VM *still* uses reference counting!
- Minimalism: Not floats! Everything is a word, just cast differently. There are no bytes, so to speak. Instructions are small, compact, and to-the-point.

### Features that RuppVM will 'PROBABLY' have:

- Ahead-of-time Compilation: AoT allows for the enter stack to be pre-compiled into machine bytecode. Not all of it, just the 'hotzones'.
- Just-in-time Compilation: JiT does the same, but during execution.

If you stumble upon this at any stage of development and have thoughts about RuppVM, contact me at `chubakbidpaa [at] riseup [dot] net`.


