#ifndef MENUCONNECT_H
#define MENUCONNECT_H

#include <QMenu>
#include "enums.h"
#include "modbusmultiserver.h"

class MenuConnect : public QMenu
{
    Q_OBJECT
public:
    enum MenuType
    {
        ConnectMenu = 0,
        DisconnectMenu
    };

    explicit MenuConnect(MenuType type, ModbusMultiServer& server, QWidget *parent = nullptr);

signals:
    void connectAction(ConnectionType type, const QString& port);
    void disconnectAction(ConnectionType type, const QString& port);

private:
    void addAction(const QString& text, ConnectionType type, const QString& port);

private:
    MenuType _menuType;
    ModbusMultiServer& _mbMultiServer;
};

#endif // MENUCONNECT_H
