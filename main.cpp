#include <QApplication>
#include "VentanaLogin.h"
#include "VentanaPrincipal.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    VentanaLogin login;
    if (login.exec() == QDialog::Accepted) {
        auto usuario = login.getUsuarioAutenticado();
        VentanaPrincipal ventanaPrincipal(usuario);
        ventanaPrincipal.show();

        return app.exec();
    }

    return 0;
}