/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/
#ifndef DEVICEMANAGERMODEL_H
#define DEVICEMANAGERMODEL_H

#include "remotelinux_export.h"

#include <QAbstractListModel>
#include <QSharedPointer>

namespace RemoteLinux {
namespace Internal { class DeviceManagerModelPrivate; }
class LinuxDeviceConfiguration;
class LinuxDeviceConfigurations;

class REMOTELINUX_EXPORT DeviceManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DeviceManagerModel(const LinuxDeviceConfigurations *deviceManager,
        QObject *parent = 0);
    ~DeviceManagerModel();

private slots:
    void handleDeviceAdded(const QSharedPointer<const LinuxDeviceConfiguration> &device);
    void handleDeviceRemoved(int idx);
    void handleDataChanged(int idx);
    void handleDeviceListChanged();

private:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Internal::DeviceManagerModelPrivate * const d;
};

} // namespace RemoteLinux

#endif // DEVICEMANAGERMODEL_H
