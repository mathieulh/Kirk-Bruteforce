# Kirk-Bruteforce

        ____       __      ____
       /\  _`\    /\ \    /\  _`\
       \ \ \/\_\  \_\ \___\ \ \/\ \
        \ \ \/_/_/\___  __\\ \ \ \ \
         \ \ \L\ \/__/\ \_/ \ \ \_\ \
          \ \____/   \ \_\   \ \____/
           \/___/     \/_/    \/___/


This is the kirk bruteforce application that was originally written to time attack and forge the Pandora IPL block

Copy the kirk_bruteforcer/kirk_bruteforcer% to the game folder.(1.5 mode if you have custom fw)

Make a folder called kirk on the root of the ms. Copy kirk/config.txt to
ms0:/kirk/config.txt. Copy the ipl block to be brute forced to kirk/brute.bin.

When its done, you'll see either ms0:/kirk/hash_done.txt, which mean a hash was
found, copy the hash to the appropriate CURRENT_HASH{0 to 7} = in the config.
Then you can start working on the next block, just save the config and continue

If you find hash_not_done.txt, it'll tell you that the hash wasn't found, until
we find the correct hash and you add it to the config, you'll have to stop.


Its probably best to disable any seplugins/remove the umd from the drive. (It can't hurt)

CREDITS
- Please do not edit or remove these credits... ;)
- All work done by the Prometheus team aka Team C+D:

  Adrahil (VoidPointer)
  Booster
  Cswindle (Caretaker)
  Dark_AleX (Malyot)
  Ditlew
  Fanjita (FullerMonty)
  Joek2100 (CosmicOverSoul)
  Jim
  Mathieulh (WiseFellow)
  Nem (h1ckeyph0rce)
  Psp250
  Skylark
  TyRaNiD (bockscar)

- With thanks to everyone who has contributed to the PSPSDK, without which
  nothing would have been possible.