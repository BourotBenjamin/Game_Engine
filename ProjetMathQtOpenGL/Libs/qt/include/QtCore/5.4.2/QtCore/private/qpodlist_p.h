/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPODLIST_P_H
#define QPODLIST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE


template <typename T, int Prealloc>
class QPodList : public QVarLengthArray<T, Prealloc>
{
    using QVarLengthArray<T, Prealloc>::s;
    using QVarLengthArray<T, Prealloc>::a;
    using QVarLengthArray<T, Prealloc>::ptr;
    using QVarLengthArray<T, Prealloc>::realloc;
public:
    inline explicit QPodList(int size = 0)
        : QVarLengthArray<T, Prealloc>(size)
    {}

    inline void insert(int idx, const T &t)
    {
        const int sz = s++;
        if (s == a)
            realloc(s, s << 1);
        ::memmove(ptr + idx + 1, ptr + idx, (sz - idx) * sizeof(T));
        ptr[idx] = t;
    }

    inline void removeAll(const T &t)
    {
        int i = 0;
        for (int j = 0; j < s; ++j) {
            if (ptr[j] != t)
                ptr[i++] = ptr[j];
        }
        s = i;
    }

    inline void removeAt(int idx)
    {
        Q_ASSERT(idx >= 0 && idx < s);
        ::memmove(ptr + idx, ptr + idx + 1, (s - idx - 1) * sizeof(T));
        --s;
    }

    inline T takeFirst()
    {
        Q_ASSERT(s > 0);
        T tmp = ptr[0];
        removeAt(0);
        return tmp;
    }
};

QT_END_NAMESPACE

#endif // QPODLIST_P_H
