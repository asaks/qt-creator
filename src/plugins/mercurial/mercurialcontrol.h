/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Brian McGillion
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

#ifndef MERCURIALCONTROL_H
#define MERCURIALCONTROL_H

#include <coreplugin/iversioncontrol.h>

namespace Mercurial {
namespace Internal {

class MercurialClient;

//Implements just the basics of the Version Control Interface
//MercurialClient handles all the work
class MercurialControl: public Core::IVersionControl
{
    Q_OBJECT
public:
    explicit MercurialControl(MercurialClient *mercurialClient);

    QString name() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool managesDirectory(const QString &filename) const;
    QString findTopLevelForDirectory(const QString &directory) const;
    bool supportsOperation(Operation operation) const;
    bool vcsOpen(const QString &fileName);
    bool vcsAdd(const QString &filename);
    bool vcsDelete(const QString &filename);
    bool sccManaged(const QString &filename);

signals:
    void enabledChanged(bool);

private:
    MercurialClient *mercurialClient;
    bool mercurialEnabled;
};

} //namespace Internal
} //namespace Mercurial

#endif // MERCURIALCONTROL_H
