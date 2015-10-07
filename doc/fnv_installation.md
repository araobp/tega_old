Fowler Noll Vo hash function
============================

home page
---------
FNV1 and FNV1a source code can be obtained from http://www.isthe.com/chongo/tech/comp/fnv/

Installation
------------
<pre>
$ wget http://www.isthe.com/chongo/src/fnv/fnv-5.0.3.tar.gz
$ unzip fnv-5.0.3.tar.gz
$ tar xvf fnv-5.0.3.tar
$ make
$ cp fnv.h /usr/local/include
$ cp longlong.h /usr/local/include
$ cp libfnv.a /usr/local/lib
</pre>

Linking the library to your code
--------------------------------
If your code's name is "fnv_test.c", then:
<pre>
$ gcc fnv_test.c -ofnv_test -lfnv
</pre>
