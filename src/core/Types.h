#pragma once 

#include <QObject>
#include <gsl/gsl>

using gsl::not_null;

template <typename Signature>
using Fn = std::function<Signature>;

using int8 = qint8;
using uint8 = quint8;

using int16 = qint16;
using uint16 = quint16;

using int32 = qint32;
using uint32 = quint32;

using int64 = qint64;
using uint64 = quint64;

using float32 = float;
using float64 = double;