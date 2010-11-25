/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
#ifndef GLSLSYMBOLS_H
#define GLSLSYMBOLS_H

#include "glsltype.h"
#include "glslsymbol.h"
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QHash>

namespace GLSL {

class GLSL_EXPORT Argument: public Symbol
{
public:
    Argument(Function *scope);

    virtual const Type *type() const;
    void setType(const Type *type);

    virtual Argument *asArgument() { return this; }

private:
    const Type *_type;
};

class GLSL_EXPORT Variable: public Symbol
{
public:
    Variable(Scope *scope);

    virtual const Type *type() const;
    void setType(const Type *type);

    virtual Variable *asVariable() { return this; }

private:
    const Type *_type;
};

class GLSL_EXPORT Block: public Scope
{
public:
    Block(Scope *enclosingScope = 0);

    void add(Symbol *symbol);

    virtual Block *asBlock() { return this; }

    virtual const Type *type() const;
    virtual Symbol *find(const QString &name) const;

private:
    QHash<QString, Symbol *> _members;
};

} // end of namespace GLSL

#endif // GLSLSYMBOLS_H
