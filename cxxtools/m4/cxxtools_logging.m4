AC_DEFUN([AC_CXXTOOLS_WITH_LOGGING],
[
  AC_ARG_WITH(
     [logging],
     AS_HELP_STRING([--with-logging],
                    [specify logging-library. Accepted arguments:
                     log4cxx, log4cplus, cxxtools, disable or probe. Default is cxxtools]),
     [ ac_loggertype=$withval ],
     [ ac_loggertype=cxxtools ])

  if test "$ac_loggertype" = "log4cxx" -o "$ac_loggertype" = "probe"; then
     AC_CHECK_HEADER(
        [log4cxx/logger.h],
        [ AC_DEFINE([CXXTOOLS_LOG_LOG4CXX],
                    [],
                    [use log4cxx for logging])
          ac_loggertype=log4cxx ],
        [ if test "$ac_loggertype" = "log4cxx"; then
            AC_MSG_ERROR(log4cxx not available)
          fi
        ]
     )
  fi

  if test "$ac_loggertype" = "log4cplus" -o "$ac_loggertype" = "probe"; then
     AC_CHECK_HEADER(
        [log4cplus/logger.h],
        [ AC_DEFINE([CXXTOOLS_LOG_LOG4CPLUS],
                    [],
                    [use log4cplus for logging])
          ac_loggertype=log4cplus ],
        [ if test "$ac_loggertype" = "log4cplus"; then
            AC_MSG_ERROR(log4cplus not available)
          fi
        ]
     )
  fi

  if test "$ac_loggertype" = "cxxtools" -o "$ac_loggertype" = "probe"; then
    AC_DEFINE([CXXTOOLS_LOG_LOGCXXTOOLS], [], [use cxxtools logging])
    ac_loggertype=cxxtools
  elif test "$ac_loggertype" = "disable" -o "$ac_loggertype" = "no"; then
    AC_DEFINE([CXXTOOLS_LOG_DISABLE], [], [disable logging])
  fi

  case "$ac_loggertype" in
      log4cxx)
          AC_MSG_NOTICE([loggertype is log4cxx])
          CXXTOOLS_LOG_LDFLAGS=-llog4cxx
          CXXTOOLS_LOGGING=CXXTOOLS_LOGGING_LOG4CXX
          ;;

      log4cplus)
          AC_MSG_NOTICE([loggertype is log4cplus])
          CXXTOOLS_LOG_LDFLAGS=-llog4cplus
          CXXTOOLS_LOGGING=CXXTOOLS_LOGGING_LOG4CPLUS
          ;;

      cxxtools)
          AC_MSG_NOTICE([loggertype is cxxtools])
          CXXTOOLS_LOG_LDFLAGS=
          CXXTOOLS_LOGGING=CXXTOOLS_LOGGING_CXXTOOLS
          ;;

      disable)
          AC_MSG_NOTICE([logging is disabled])
          CXXTOOLS_LOGGING=CXXTOOLS_LOGGING_DISABLE
          ;;

      *)
          AC_MSG_ERROR([unknown loggertype $ac_loggertype])
          ;;
  esac
])
