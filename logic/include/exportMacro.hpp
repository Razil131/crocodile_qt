#pragma once

#include <QtCore/qglobal.h>

#if defined(CROCODILE_BACK_LIBRARY)
#  define CROCODILE_BACK_EXPORT Q_DECL_EXPORT
#else
#  define CROCODILE_BACK_EXPORT Q_DECL_IMPORT
#endif