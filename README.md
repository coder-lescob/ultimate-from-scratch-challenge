# Ultimate Challenge

## The first challenge

This is an ultimate programming challenge. Using this distro to make anything usable here is the challenge, this distro is the most minimalist linux distro ever. It literally only consist of a minimalist custom shell. This shell can only run commands and it cannot interpret a bash script for example, no, no, when you type a command it just calls /bin/your-command with the given arguments. However, redirection of stdin and stdout/stderr is possible through the familiar syntax. Furthermore it runs inside a riscv32 qemu vm and has only basic tooling such as printf, cd, ls, exit, rm, cat and that's it! No compiler, no assembler. Just a raw shell. But obviously the challenge is still to code. Good luck!

## The second challenge

For anyone more interested in hacking, I've not intentionally left vulnerabilities in the codebase, but if you find some please share it to us in the issues, we'll maybe not fix it but finding it would involve a challenge so it would be part of this repo's reach. Bonus points if you can get code execution using a vulnerability. Good luck! 

## Use it

In order to do this challenge, you must first clone this repo:

```Shell
git clone https://github.com/coder-lescob/ultimate-from-scratch-challenge
```

Then launch the `setup.sh` script:

```Shell
./setup.sh
```

This script will make sure you have all tools necessary installed, and it will compile the kernel which may take a while. Then just run the makefile with:

```Shell
make run
```

And a qemu window should appear with my beautiful shell inside.

## Objectives

The objective of this project is purely educative both for it's developers and for it's users. As it is, no AI pull request will be accepted. That said, the next things to do is to put the distro in a usable disk image that could be later used to lauch qemu and even on real hardware.
