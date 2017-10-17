AC_PREREQ([2.68])

# set name and version
AC_INIT( [libblockparty], [1.0] )

# allow foreign syntax
AM_INIT_AUTOMAKE( [foreign 1.13] )

# add custom search path
LDFLAGS="$LDFLAGS -L/usr/lib/x86_64-linux-gnu/"

###########################################################
### Libraries #############################################
###########################################################

# C library search (note: don't use pkg-config)
echo " "
echo " >> Checking for C libraries (or libraries with C only symbols)"
echo "---------------------------------------------------------------"
AC_SEARCH_LIBS([dlopen],                     [dl],            [], [AC_MSG_ERROR([unable to find ldl]) ])
AC_SEARCH_LIBS([mysql_init],                 [mariadb],       [], [AC_MSG_ERROR([unable to find libmariadb]) ])
AC_SEARCH_LIBS([MHD_start_daemon],           [microhttpd],    [], [AC_MSG_ERROR([unable to find libmicrohttpd]) ])
AC_SEARCH_LIBS([login],                      [util],          [], [AC_MSG_ERROR([unable to find libutil]) ])
AC_SEARCH_LIBS([pcre_compile],               [pcre2],         [], [AC_MSG_ERROR([unable to find libpcre2]) ])
AC_SEARCH_LIBS([SSL_library_init],           [ssl],           [], [AC_MSG_ERROR([unable to find libssl]) ])
AC_SEARCH_LIBS([DH_free],                    [crypto],        [], [AC_MSG_ERROR([unable to find libcrypto]) ])
AC_SEARCH_LIBS([pthread_create],             [pthread],       [], [AC_MSG_ERROR([unable to find libpthread]) ])
AC_SEARCH_LIBS([curl_easy_init],             [curl],          [], [AC_MSG_ERROR([unable to find libcurl]) ])
AC_SEARCH_LIBS([inflate],                    [z],             [], [AC_MSG_ERROR([unable to find zlib (libz)]) ])
AC_SEARCH_LIBS([htmlReadMemory],             [xml2],          [], [AC_MSG_ERROR([unable to find libxml2]) ])
AC_SEARCH_LIBS([sg_snapshot],                [statgrab],      [], [AC_MSG_ERROR([unable to find libstatgrab]) ])
AC_SEARCH_LIBS([sqlite3_threadsafe],         [sqlite3],       [], [AC_MSG_ERROR([unable to find libsqlite3]) ])
AC_SEARCH_LIBS([gnutls_record_send],         [gnutls],        [], [AC_MSG_ERROR([unable to find libgnutls]) ])


# C++ library search (only possible using pkg-config)
echo " "
echo " >> Checking for C++ only libraries."
echo "------------------------------------"
PKG_CHECK_MODULES([vmime],      [vmime > 0.1],       [ AC_MSG_NOTICE([pkg-config found libvmime])    ], [ AC_MSG_ERROR([pkg_check can't fine libvmime (c++ library)]) ])
PKG_CHECK_MODULES([mozjs_31],   [mozjs-31 > 0.1.1],  [ AC_MSG_NOTICE([pkg-config found libmozjs-31]) ], [ AC_MSG_ERROR([pkg_check can't fine libmozjs-31]) ])



# check for compiler/ranlib
echo " "
echo " >> Checking for C++ compiler and ranlib."
echo "-------------------------------------------"

# check for c++ compiler
AC_PROG_CXX

# check for ranlib
AC_PROG_RANLIB

# generate config.h header
AC_CONFIG_HEADERS( [config.h] )


# check for compiler/ranlib
echo " "
echo " >> Creating configure script, now run ./configure && make to build library."
echo "----------------------------------------------------------------------------"


# set config file and output results
AC_CONFIG_FILES( [Makefile] )
AC_OUTPUT
