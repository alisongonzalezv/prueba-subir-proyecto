#ifndef VISTAVENTAS_H
#define VISTAVENTAS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTextBrowser>
#include <QDialog>
#include <QDateTime>
#include <memory>
#include <vector>
#include "IGeneradorTicket.h"

struct ProductoItem {
    QString id;
    QString nombre;
    double precio;
    QString descripcion;
    QString categoria;
    int stock;
};

struct ItemCarrito {
    ProductoItem producto;
    int cantidad;
};

class VistaVentas : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtBuscar;
    QVBoxLayout* layoutListaCarrito;
    QWidget* contenedorCarrito;
    QGridLayout* gridProductos;

    // Desglose de Totales y Descuento
    QLabel* lblSubtotal;
    QLabel* lblIVA;
    QComboBox* comboTipoDescuento;
    QDoubleSpinBox* spinDescuento;
    QLabel* lblTotal;
    QLabel* lblResumenItems;

    // Pago en Efectivo
    QDoubleSpinBox* spinMontoPaga;
    QLabel* lblCambio;
    QPushButton* btnCheckout;
    QPushButton* btnVaciarCarrito;

    // Estado Interno
    std::vector<ProductoItem> catalogo;
    std::vector<ItemCarrito> carrito;
    QString categoriaSeleccionada;

    double subtotalActual;
    double ivaActual;
    double descuentoActual;
    double totalActual;
    int contadorVentas;

public:
    explicit VistaVentas(QWidget* parent = nullptr);

private slots:
    void cambiarCategoriaSlot(const QString& cat);

private:
    void cargarCatalogoDemo();
    void configurarInterfaz();

    void renderizarProductos();
    void renderizarCarrito();

    QWidget* crearTarjetaProducto(const ProductoItem& prod);
    QWidget* crearWidgetCarritoItem(const ItemCarrito& item, int index);

    void agregarAlCarrito(const ProductoItem& prod);
    void cambiarCantidadItem(int index, int delta);
    void eliminarDelCarrito(int index);
    void vaciarCarrito();
    void actualizarTotales();
    void calcularCambioEfectivo();
    void filtrarProductos();
    void procesarCheckout();

    void mostrarYGuardarTicket(const DatosVentaTicket& datosVenta);
};

#endif // VISTAVENTAS_H