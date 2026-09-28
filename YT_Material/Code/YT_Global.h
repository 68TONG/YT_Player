#pragma once
#include <QtGlobal>

#if defined(YT_BUILD_STATIC)
#  define YT_EXPORT                  
#elif defined(YT_BUILD_SHARED)
#  define YT_EXPORT Q_DECL_EXPORT   
#else
#  define YT_EXPORT Q_DECL_IMPORT   
#endif