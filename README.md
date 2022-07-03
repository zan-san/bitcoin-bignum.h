# bitcoin-bignum.h
help you use `bitcoin/bignum.h` with `openssl1.1.1`

# description
in this page [openssl1.1.1 man page](https://www.openssl.org/docs/man1.1.1/man3/BN_clear.html). 

it said that 

```BN_init() was removed in OpenSSL 1.1.0; use BN_new() instead.```

so the lib `bitcoin/bignum.h` can not use with openssl1.1.1 or  after anymore.

but I fix `bignum.h` in this repository

you can use this lib file replace  `bitcoin/bignum.h`

and it can work both `openssl 1.0.2` or `openssl1.1.1`

so there are no more problem with `BN_init()`.

have fun　:) 

楽しみましょう
